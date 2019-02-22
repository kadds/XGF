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
		virtual void Exec() = 0;
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
	enum class RenderGroupType
	{
		Background,
		Normal,
		Transparent,
		Foreground,
	};
	struct RenderTargetPass
	{
		RenderTargetPass() : mRenderQueues(4), mIsClearTarget(false), mClearDepthStencilBuffer(false){  }
		~RenderTargetPass();

		void Clear(const Color& color)
		{
			mClearColor = color;
		}
		void ClearDepthStencilBuffer()
		{
			mClearDepthStencilBuffer = true;
		}
		Color mClearColor;
		FrameBuffer * mTarget;
		bool mClearDepthStencilBuffer;
		bool mIsClearTarget;
		std::vector<RenderQueue> mRenderQueues;
	};
	struct RendererFrameResource
	{
		std::list<GpuBuffer> mVertices, mIndices;
		std::list<GpuBuffer> mVerticesUsed, mIndicesUsed;
		std::list<GpuBuffer> mConstantBuffer, mConstantBufferUsed;
		int mPassUsedIndex;
		std::vector<RenderTargetPass *> mPass;
		RenderTargetPass mDefaultPass;
		unsigned int mRenderIndex;
		RenderTargetPass& GetCurrentPass();
		bool mRenderedFlag;
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
		RendererFrameResource(): mRenderedFlag(false), mPassUsedIndex(-1){  }
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
		void ClearDepthStencilBuffer();
		void Clear(const Color& color);
		void Create();
		void Destroy();

		void Commit(RenderGroupType groupType, RenderCommand* cmd);

		void Commit(int groupIndex, RenderCommand* cmd);
		bool IsPresentAllResource();
		RenderQueue& GetRenderGroup(int index);
		RenderQueue& GetRenderGroup(RenderGroupType groupType);
		void Loop();
		GpuBuffer QueryUnusedGpuIndexBuffer(unsigned count);
		GpuBuffer QueryUnusedGpuVertexBuffer(unsigned size);
		GpuBuffer QueryUnusedGpuConstantBuffer(unsigned size);

		void SetLimitFrameRate(int frameRate);
		int GetLimitFrameRate() const;

		int AppendFrameTarget(FrameBuffer* target);
		int AppendDefaultFrameTarget();
		void AppendAndSetDefaultFrameTarget();
		void SetDefaultFrameTarget();
		void SetFrameTarget(int index);
		void SetFrameTarget(FrameBuffer* target);
		void AppendAndSetFrameTarget(FrameBuffer* target);
		FrameBuffer * GetCurrentFrameTarget();

		void AppendBeforeDrawCallback(const std::string & name, RendererDrawCallbackFunc callback);
		void AppendAfterDrawCallback(const std::string & name, RendererDrawCallbackFunc callback);
		void RemoveBeforeDrawCallback(const std::string & name);
		void RemoveAfterDrawCallback(const std::string & name);

		void AppendBeforeDrawCallbackOnce(RendererDrawCallbackFunc callback);
		void AppendAfterDrawCallbackOnce(RendererDrawCallbackFunc callback);

		bool IsRenderStart() const;
		bool IsRenderEnd() const;

		void WaitFrame();
	private:
		void DrawCommands();
		void BeforeDraw();
		void AfterDraw();
		void OnMessage(const Event & ev);
		void ClearResource(int index);
		FrameRateLimiter mFrameRateLimiter;
		RendererFrameResource mResource[2];
		int mIndexOfResource;
		std::atomic_bool mTag;
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

		RendererFrameResource& GetCurrentRenderResource();
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
			auto buffer = renderer.QueryUnusedGpuConstantBuffer(mRenderStage.GetShader<TShader>()->GetCBufferSize(index));
			gdi.CopyToBuffer(buffer, cb[index].GetBufferPoint());
			BindBuffer<TShader>(&gdi, index, buffer);
		}

		DefaultRenderCommand(char * vertices, unsigned int vertexCount, unsigned vertexSize, Index * indices, unsigned int indexCount, const RenderStage& renderStage) :
			mVertices(vertices), mVertexCount(vertexCount), mVertexSize(vertexSize), mIndices(indices), mIndexCount(indexCount), mRenderStage(renderStage) {  };
		static DefaultRenderCommand* MakeRenderCommand(const BindingBridge& bbr, const PolygonPleIndex& index,
			const RenderStage& renderStage);

		~DefaultRenderCommand();
		virtual void Exec() override;
	};

}
