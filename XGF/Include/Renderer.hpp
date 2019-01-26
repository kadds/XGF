#pragma once
#include "Defines.hpp"
#include "Shader.hpp"
#include "FrameRateLimiter.hpp"


namespace XGF
{
	class Event;
	class PolygonPleIndex;
	class BindingBridge;

	class RenderCommand
	{
	protected:
		int mRenderOrder;
	public:
		RenderCommand():mRenderOrder(0) {  }
		RenderCommand(int order): mRenderOrder(order) {  }
		virtual ~RenderCommand() {  }
		virtual void Exec() = 0;
		int GetRenderOrder() const
		{
			return mRenderOrder;
		}

	};
	class DefaultRenderCommand : public RenderCommand
	{
	private:
		ShaderStage mShaderStage;
		char * mVertices;
		unsigned int mVertexCount;
		Index * mIndices;
		unsigned int mIndexCount;
		unsigned mVertexSize;
	public:
		void BindVSConstantBuffer(unsigned index);
		void BindPSConstantBuffer(unsigned index);
		void BindGSConstantBuffer(unsigned index);
		DefaultRenderCommand(char * vertices, unsigned int vertexCount, unsigned vertexSize,  Index * indices, unsigned int indexCount):
		mVertices(vertices), mVertexCount(vertexCount), mVertexSize(vertexSize), mIndices(indices), mIndexCount(indexCount){  };
		static DefaultRenderCommand* MakeRenderCommand(const BindingBridge& bbr, const PolygonPleIndex& index,
		                                               const ShaderStage& shaderStage);

		~DefaultRenderCommand();
		virtual void Exec() override;
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
		RenderTarget * mTarget;
		bool mClearDepthStencilBuffer;
		bool mIsClearTarget;
		std::vector<RenderQueue> mRenderQueues;
	};
	struct RenderResource
	{
		std::list<GpuBuffer> mVertices, mIndices;
		std::list<GpuBuffer> mVerticesUsed, mIndicesUsed;
		std::list<GpuBuffer> mConstantBuffer, mConstantBufferUsed;
		int mPassUsedIndex;
		std::vector<RenderTargetPass *> mPass;
		RenderTargetPass mDefaultPass;

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

		void NewFrame();
		RenderResource(): mRenderedFlag(false), mPassUsedIndex(-1){  }
	};
	
	class Renderer
	{
	public:
		Renderer() = default;
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

		int AppendRenderTarget(RenderTarget* target);
		int AppendDefaultRenderTarget();
		void AppendAndSetDefaultRenderTarget();
		void SetDefaultRenderTarget();
		void SetRenderTarget(int index);
		void AppendAndSetRenderTarget(RenderTarget* target);
	private:
		void DrawCommands();
		void OnMessage(const Event & ev);
		void ClearResource(int index);
		FrameRateLimiter mFrameRateLimiter;
		RenderResource mResource[2];
		int mIndexOfResource;
		std::atomic_bool mTag;
		RenderResource& GetCurrentResource();

		int GetNextResourceIndex() const;

		int GetCurrentResourceIndex() const;

		RenderResource& GetResource(int index);

		void SetCurrentResource(int index);

		RenderResource& GetCurrentRenderResource();
	};
}
