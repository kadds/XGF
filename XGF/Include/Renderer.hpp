#pragma once
#include "Defines.hpp"
#include "Shader.hpp"
#include "FrameRateLimiter.hpp"
#include <atomic>
#include <mutex>
#include "Context.hpp"


namespace XGF
{
	class Event;
	class PolygonPleIndex;
	class BindingBridge;
	class Renderer;

	class RenderCommand
	{
	protected:
		int mRenderOrder;
		// Number.
		unsigned int mRenderIndex;
	public:
		RenderCommand() :mRenderOrder(0){  }
		RenderCommand(int order) : mRenderOrder(order) {  }
		virtual ~RenderCommand() {  }
		virtual void Exec(GDI & gdi, FrameBuffer & frameBuffer) = 0;
		int GetRenderOrder() const
		{
			return mRenderOrder;
		}
		void SetRenderIndex (unsigned int index)
		{
			mRenderIndex = index;
		}
	};
	class RenderQueue
	{
	public:
		void Reorder();
		std::deque<RenderCommand*> commands;
		void Push(RenderCommand* cmd);
	};
	typedef unsigned RenderGroupRawType;
	enum class RenderGroupType : RenderGroupRawType
	{
		Background,
		Normal,
		Transparent,
		Foreground,
	};
	
	struct RenderTargetPass
	{
		RenderTargetPass(FrameBuffer * target) : mRenderQueues(4), mTarget(target){  }
		~RenderTargetPass() = default;
		void Clear();

		FrameBuffer * mTarget;

		std::vector<RenderQueue> mRenderQueues;
	};
	class RendererFrameResource
	{
	private:
		std::list<GpuBuffer> mVertices, mIndices;
		std::list<GpuBuffer> mVerticesUsed, mIndicesUsed;
		std::list<GpuBuffer> mConstantBuffer, mConstantBufferUsed;
		std::vector<RenderTargetPass> mPass;
		std::stack<unsigned> mPassStack;
		unsigned int mRenderIndex;
		bool mRenderedFlag;

	public:
		unsigned int NextRenderIndex();
		std::vector<RenderTargetPass>& GetAllPasses();
		RenderTargetPass& GetCurrentPass();
		void Push(FrameBuffer* frameBuffer);

		FrameBuffer* Pop();

		bool HasRenderedFlag() const
		{
			return mRenderedFlag;
		}
		void ClearRenderedFlag()
		{
			mRenderedFlag = false;
		}
		void SetRenderedFlag()
		{
			mRenderedFlag = true;
		}
		bool HasCommands() const
		{
			return !mPass.empty();
		}
		void NewFrame();
		void ClearAll();
		GpuBuffer QueryUnusedGpuIndexBuffer(unsigned count);
		GpuBuffer QueryUnusedGpuVertexBuffer(unsigned size);
		GpuBuffer QueryUnusedGpuConstantBuffer(unsigned size);
		RendererFrameResource(): mRenderedFlag(false){  }
	};

	typedef std::function<void(Renderer *)> RendererDrawCallbackFunc;

	struct DrawCallback
	{
		int callCount;
		RendererDrawCallbackFunc func;
		std::string name;

		DrawCallback(RendererDrawCallbackFunc func, const std::string & name, int callCount): callCount(callCount), func(func), name(name) {  }
	};
	class Renderer
	{
	public:
		Renderer() = default;
		void ClearDepthStencilBuffer(RenderGroupType groupType = RenderGroupType::Normal);
		void Clear(const Color& color, RenderGroupType groupType = RenderGroupType::Normal);
		void ClearColor(const Color& color, RenderGroupType groupType = RenderGroupType::Normal);

		void ClearDepthStencilBuffer(RenderGroupRawType groupType);
		void Clear(const Color& color, RenderGroupRawType groupType);
		void ClearColor(const Color& color, RenderGroupRawType groupType);

		void Create();
		void Destroy();

		void Commit(RenderGroupType groupType, RenderCommand* cmd);

		void Commit(RenderGroupRawType groupIndex, RenderCommand* cmd);

		RenderQueue& GetRenderGroup(RenderGroupRawType index);
		RenderQueue& GetRenderGroup(RenderGroupType groupType);
		void Loop();

		void SetLimitFrameRate(int frameRate);
		int GetLimitFrameRate() const;

		FrameBuffer * GetCurrentFrameTarget();

		void PushFrameTarget(FrameBuffer * target);
		void PushDefaultFrameTarget();
		FrameBuffer * PopFrameTarget();

		// pop and set a new frame target at last
		FrameBuffer * PopNewFrameTarget();

		void AppendBeforeDrawCallback(const std::string & name, RendererDrawCallbackFunc callback);
		void AppendAfterDrawCallback(const std::string & name, RendererDrawCallbackFunc callback);
		void RemoveBeforeDrawCallback(const std::string & name);
		void RemoveAfterDrawCallback(const std::string & name);

		void AppendBeforeDrawCallbackOnce(RendererDrawCallbackFunc callback);
		void AppendAfterDrawCallbackOnce(RendererDrawCallbackFunc callback);

		bool IsRenderStart() const;
		bool IsRenderEnd() const;

		void WaitFrame();

		RendererFrameResource& GetCurrentRenderResource();
	private:
		void DrawCommands();
		void BeforeDraw();
		void AfterDraw();
		void OnMessage(const Event & ev);
		void ClearResource(int index);
		FrameRateLimiter mFrameRateLimiter;
		RendererFrameResource mResource[2];
		int mIndexOfResource;
		bool mLooping;
		bool mLooped;
		std::atomic_bool mDrawing;

		std::recursive_mutex mDrawMutex;
		std::vector<DrawCallback> mBeforeDrawCallback;
		std::vector<std::tuple<DrawCallback, bool>> mBeforeDrawCallbackTemp;
		std::vector<DrawCallback> mAfterDrawCallback;
		std::vector<std::tuple<DrawCallback, bool>> mAfterDrawCallbackTemp;

		RendererFrameResource& GetCurrentResource();

		int GetNextResourceIndex() const;

		int GetCurrentResourceIndex() const;

		RendererFrameResource& GetResource(int index);

		void SetCurrentResource(int index);

		
	};

	class ClearRenderCommand : public RenderCommand
	{
	private:
		Color mClearColor;
		bool mClearTarget;
		bool mClearDepth;
	public:
		void Exec(GDI & gdi, FrameBuffer & frameBuffer) override;
		ClearRenderCommand(const Color & clearColor, bool clearTarget, bool clearDepth): mClearColor(clearColor), mClearTarget(clearTarget), mClearDepth(clearDepth) {  }
		static ClearRenderCommand * MakeRenderCommand(const Color & clearColor, bool clearDepth);
		static ClearRenderCommand * MakeRenderCommand(bool clearDepth);
	};

	class SubRenderCommand final : public RenderCommand
	{
	private:
		std::vector<RenderCommand *> mCommands;

	public:
		SubRenderCommand(std::vector<RenderCommand *> && commands): mCommands(std::move(commands)) {  }
		~SubRenderCommand();
		void Exec(GDI& gdi, FrameBuffer& frameBuffer) override;
		template<typename Head, typename...TArgs>
		static void UnpackCommands(RenderCommand ** commandPtr, Head&& head, TArgs&&...args)
		{
			*commandPtr = head;
			UnpackCommands(++commandPtr, std::forward<TArgs>(args));
		}
		template<typename Head>
		static void UnpackCommands(RenderCommand ** commandPtr, Head&& head)
		{
			*commandPtr = head;
		}
		template<typename ...TArgs>
		static SubRenderCommand * MakeRenderCommand(TArgs&&...args)
		{
			std::vector<RenderCommand *> commands(sizeof...(args));
			UnpackCommands(&commands[0], std::forward<TArgs>(args));
			return new SubRenderCommand(std::move(commands));
		}

		static SubRenderCommand* MakeRenderCommand(std::vector<RenderCommand *>&& commands);
	};

	class DefaultRenderCommand : public RenderCommand
	{
	private:
		RawRenderStage mRenderStage;
		char * mVertices;
		unsigned int mVertexCount;
		Index * mIndices;
		unsigned int mIndexCount;
		unsigned mVertexSize;
	private:
		template<typename TShader>
		void BindBuffer(GDI * gdi, unsigned index, GpuBuffer & buffer)
		{
			static_assert(false, "null shader type");
		}
		template<>
		void BindBuffer<VertexShader>(GDI * gdi, unsigned index, GpuBuffer & buffer)
		{
			gdi->GetDeviceContext()->VSSetConstantBuffers(mRenderStage.GetShader<VertexShader>()->GetCBufferSlot(index), 1, &buffer.buffer);
		}
		template<>
		void BindBuffer<PixelShader>(GDI * gdi, unsigned index, GpuBuffer & buffer)
		{
			gdi->GetDeviceContext()->PSSetConstantBuffers(mRenderStage.GetShader<PixelShader>()->GetCBufferSlot(index), 1, &buffer.buffer);
		}
		template<>
		void BindBuffer<GeometryShader>(GDI * gdi, unsigned index, GpuBuffer & buffer)
		{
			gdi->GetDeviceContext()->GSSetConstantBuffers(mRenderStage.GetShader<GeometryShader>()->GetCBufferSlot(index), 1, &buffer.buffer);
		}
	public:
		template<typename TShader>
		void BindConstantBuffer(unsigned index)
		{
			auto & context = Context::Current();
			auto & gdi = context.QueryGraphicsDeviceInterface();
			auto & renderer = context.QueryRenderer();
			auto & cb = mRenderStage.GetConstantBuffer<TShader>();
			auto buffer = renderer.GetCurrentRenderResource().QueryUnusedGpuConstantBuffer(mRenderStage.GetShader<TShader>()->GetCBufferSize(index));
			gdi.CopyToBuffer(buffer, cb[index].GetBufferPoint());
			BindBuffer<TShader>(&gdi, index, buffer);
		}

		DefaultRenderCommand(char * vertices, unsigned int vertexCount, unsigned vertexSize, Index * indices, unsigned int indexCount, const RenderStage& renderStage) :
			mVertices(vertices), mVertexCount(vertexCount), mVertexSize(vertexSize), mIndices(indices), mIndexCount(indexCount), mRenderStage(renderStage) {  };
		static DefaultRenderCommand* MakeRenderCommand(const BindingBridge& bbr, const PolygonPleIndex& index,
			const RenderStage& renderStage);

		~DefaultRenderCommand();
		virtual void Exec(GDI & gdi, FrameBuffer & frameBuffer) override;
	};

}
