#include "./../../Include/Renderer.hpp"
#include "../../Include/Asyn.hpp"
#include "../../Include/Polygon.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/DebugInscriber.hpp"
#include "../../Include/Logger.hpp"

namespace XGF
{
	DefaultRenderCommand* DefaultRenderCommand::MakeRenderCommand(const BindingBridge& bbr,
		const PolygonPleIndex& index,
		const RenderStage& renderStage)
	{
		const unsigned memsize = static_cast<unsigned>(bbr.GetAllPolygonPleMemSize());
		char* vertices = new char[memsize];
		std::vector<float> vertices1(memsize / sizeof(float));
		auto indices = new Index[index.GetActualCount()];
		auto cmd = new DefaultRenderCommand(vertices, bbr.GetBinder(0)->GetActualCount(), memsize, indices, index.GetActualCount(), renderStage);
		unsigned int start = 0u;
		unsigned int chunk = renderStage.GetRenderResource()->GetShader<VertexShader>()->GetStrideAllSizeAtSlot(0);
		const unsigned int * stride = renderStage.GetRenderResource()->GetShader<VertexShader>()->GetStride();
 		for (int i = 0; i < bbr.Count(); i++)
		{
			bbr.GetBinder(i)->CopyTo(vertices + start, chunk);
			bbr.GetBinder(i)->CopyTo((char*)( vertices1.data()) + start, chunk);
			start += stride[i];
		}
		index.CopyTo(indices, 0);
		return cmd;
	}

	DefaultRenderCommand::~DefaultRenderCommand()
	{
		delete[] mVertices;
		if(mIndexCount > 0)
			delete[] mIndices;
	}


	void DefaultRenderCommand::Exec()
	{
		unsigned int offset[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{ 0 };

		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		mRenderStage.BindStage();
		if (!mRenderStage.GetConstantBuffer<VertexShader>().empty())
			for (int i = 0; i < mRenderStage.GetShader<VertexShader>()->GetCBufferCount(); i++)
				BindConstantBuffer<VertexShader>(i);
		if (!mRenderStage.GetConstantBuffer<PixelShader>().empty())
			for (int i = 0; i < mRenderStage.GetShader<PixelShader>()->GetCBufferCount(); i++)
				BindConstantBuffer<PixelShader>(i);
		if (!mRenderStage.GetConstantBuffer<GeometryShader>().empty())
			for (int i = 0; i < mRenderStage.GetShader<GeometryShader>()->GetCBufferCount(); i++)
				BindConstantBuffer<GeometryShader>(i);


		auto vertexBuffer = Context::Current().QueryRenderer().QueryUnusedGpuVertexBuffer(mVertexSize);
		auto stride = mRenderStage.GetShader<VertexShader>()->GetStrideAllSizeAtSlot(0);
		gdi.GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer.buffer, &stride, offset);
		memcpy_s(gdi.Map(vertexBuffer), mVertexSize, mVertices, mVertexSize);
		gdi.UnMap(vertexBuffer);
		if (mIndexCount > 0)
		{
			auto indexBuffer = Context::Current().QueryRenderer().QueryUnusedGpuIndexBuffer(mIndexCount);
			gdi.GetDeviceContext()->IASetIndexBuffer(indexBuffer.buffer, DXGI_FORMAT_R32_UINT, 0);
			memcpy_s(gdi.Map(indexBuffer), sizeof(Index) * mIndexCount, mIndices, sizeof(Index) * mIndexCount);
			gdi.UnMap(indexBuffer);
			gdi.GetDeviceContext()->DrawIndexed(mIndexCount, 0, 0);
		}
		else
		{
			gdi.GetDeviceContext()->Draw(mVertexCount, 0);
		}

		mRenderStage.UnBindStage();
	}

	void RenderQueue::Reorder()
	{
		std::sort(commands.begin(), commands.end(), [](const RenderCommand* cmd1, const RenderCommand* cmd2)
		{
			return cmd1->GetRenderOrder() < cmd2->GetRenderOrder();
		});
	}

	void RenderQueue::Push(RenderCommand* cmd)
	{
		commands.push_back(cmd);
	}

	RenderTargetPass::~RenderTargetPass()
	{
		for (auto& renderQueue : mRenderQueues)
		{
			for (auto command : renderQueue.commands)
			{
				delete command;
			}
		}
		mRenderQueues.clear();
	}

	RenderTargetPass& RendererFrameResource::GetCurrentPass()
	{
		if (mPassUsedIndex < 0)
		{
			if(mPass.empty())
			{
				// 自动添加为默认Pass
				mPassUsedIndex = 0;
				mPass.push_back(&mDefaultPass);
			}
			else
			{
				mPassUsedIndex = 0;
				XGF_Info(Render, "Pass has not been set, now set to the first pass");
			}
		}
		return *mPass[mPassUsedIndex];
	}

	void RendererFrameResource::NewFrame()
	{
		mVertices.insert(mVertices.end(), mVerticesUsed.begin(), mVerticesUsed.end());
		mVerticesUsed.clear();

		mIndices.insert(mIndices.end(), mIndicesUsed.begin(), mIndicesUsed.end());
		mIndicesUsed.clear();

		mConstantBuffer.insert(mConstantBuffer.end(), mConstantBufferUsed.begin(), mConstantBufferUsed.end());
		mConstantBufferUsed.clear();

		for (auto pass : mPass)
		{
			if(pass != &mDefaultPass)
			{
				delete pass;
			}
		}
		mPass.clear();
		mPassUsedIndex = -1;
		mRenderIndex = 0;
	}
	void Renderer::ClearDepthStencilBuffer()
	{
		auto & pass = GetCurrentResource().GetCurrentPass();
		pass.ClearDepthStencilBuffer();
	}
	void Renderer::Clear(const Color& color)
	{
		auto & pass = GetCurrentResource().GetCurrentPass();
		pass.Clear(color);
		pass.ClearDepthStencilBuffer();
	}

	void Renderer::Create()
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		gdi.Create();
		Context::Current().QueryRenderThread().SetCallBackFunc(std::bind(&Renderer::OnMessage, this, std::placeholders::_1));
		mIndexOfResource = 0;
		mTag = true;
		GetCurrentRenderResource().SetRenderedFlag();
		GetCurrentResource().SetRenderedFlag();
		GetCurrentResource().mDefaultPass.mTarget = gdi.GetDisplayFrameBuffer();
		GetCurrentRenderResource().mDefaultPass.mTarget = gdi.GetDisplayFrameBuffer();
		mLooping = false;
		mLooped = false;
		mDrawing = false;
	}

	void Renderer::Destroy()
	{
		for (auto & resource : mResource)
		{
			for (auto pass : resource.mPass)
			{
				for (auto renderQueue : pass->mRenderQueues)
				{
					for (auto command : renderQueue.commands)
					{
						delete command;
					}
				}
				pass->mRenderQueues.clear();
			}
			for (GpuBuffer & buffer : resource.mVertices)
			{
				buffer.buffer->Release();
			}
			for (GpuBuffer & buffer : resource.mVerticesUsed)
			{
				buffer.buffer->Release();
			}
			for (GpuBuffer& buffer : resource.mIndices)
			{
				buffer.buffer->Release();
			}
			for (GpuBuffer& buffer : resource.mIndicesUsed)
			{
				buffer.buffer->Release();
			}
			for (GpuBuffer & buffer : resource.mConstantBuffer)
			{
				buffer.buffer->Release();
			}
			for (GpuBuffer & buffer : resource.mConstantBufferUsed)
			{
				buffer.buffer->Release();
			}
		}

		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		gdi.Destroy();
	}

	void Renderer::AppendAndSetFrameTarget(FrameBuffer* target)
	{
		SetFrameTarget(AppendFrameTarget(target));
	}

	FrameBuffer * Renderer::GetCurrentFrameTarget()
	{
		return GetCurrentResource().GetCurrentPass().mTarget;
	}

	void Renderer::AppendBeforeDrawCallback(const std::string & name, RendererDrawCallbackFunc callback)
	{
		std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
		mBeforeDrawCallbackTemp.emplace_back(DrawCallback(callback, name, -1), true);
	}

	void Renderer::AppendAfterDrawCallback(const std::string & name, RendererDrawCallbackFunc callback)
	{
		std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
		mAfterDrawCallbackTemp.emplace_back(DrawCallback(callback, name, -1), true);
	}

	void Renderer::RemoveBeforeDrawCallback(const std::string & name)
	{
		std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
		mBeforeDrawCallbackTemp.emplace_back(DrawCallback(nullptr, name, -1), false);
	}

	void Renderer::RemoveAfterDrawCallback(const std::string & name)
	{
		std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
		mAfterDrawCallbackTemp.emplace_back(DrawCallback(nullptr, name, -1), false);
	}

	void Renderer::AppendBeforeDrawCallbackOnce(RendererDrawCallbackFunc callback)
	{
		std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
		mBeforeDrawCallbackTemp.emplace_back(DrawCallback(callback, "", 1), true);
	}

	void Renderer::AppendAfterDrawCallbackOnce(RendererDrawCallbackFunc callback)
	{
		std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
		mAfterDrawCallbackTemp.emplace_back(DrawCallback(callback, "", 1), true);
	}

	bool Renderer::IsRenderStart() const
	{
		return mLooping;
	}

	bool Renderer::IsRenderEnd() const
	{
		return mLooped;
	}

	void Renderer::WaitFrame()
	{
		while(!(!mDrawing && GetCurrentResource().HasRenderedFlag() && GetCurrentRenderResource().HasRenderedFlag()
			&& !GetCurrentResource().HasCommands()))
		{
			
		}
	}

	void Renderer::DrawCommands()
	{
		auto & context = Context::Current();
		for (auto pass : GetCurrentRenderResource().mPass)
		{
			pass->mTarget->Bind();
			pass->mTarget->Clear(pass->mClearColor);
			if(pass->mClearDepthStencilBuffer)
			{
				pass->mTarget->ClearDepthStencilBuffer();
			}
			for (auto & renderQueue : pass->mRenderQueues)
			{
				renderQueue.Reorder();
				for (auto & command : renderQueue.commands)
				{
					command->Exec();
					delete command;
				}
				renderQueue.commands.clear();
			}
		}
		context.QueryGraphicsDeviceInterface().Present(false);
	}


	void Renderer::BeforeDraw()
	{
		if(!mBeforeDrawCallbackTemp.empty())
		{
			std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
			for (auto & drawCallback : mBeforeDrawCallbackTemp)
			{
				if(std::get<bool>(drawCallback)) // append
				{
					mBeforeDrawCallback.emplace_back(std::get<DrawCallback>(drawCallback));
				}
				else // delete from vector
				{
					auto & name = std::get<DrawCallback>(drawCallback).name;
					
					auto it = std::find_if(mBeforeDrawCallback.begin(), mBeforeDrawCallback.end(), [&name](const DrawCallback & callback)
					{
						return callback.name == name;
					});
					if (it != mBeforeDrawCallback.end())
						mBeforeDrawCallback.erase(it);
				}
			}
			mBeforeDrawCallbackTemp.clear();
		}
		for (auto it = mBeforeDrawCallback.begin(); it != mBeforeDrawCallback.end();)
		{
			it->func(this);
			if (it->callCount > 0)
				it->callCount--;
			if (it->callCount == 0)
				it = mBeforeDrawCallback.erase(it);
			else
				++it;
		}
	}

	void Renderer::AfterDraw()
	{
		if (!mAfterDrawCallbackTemp.empty())
		{
			std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
			for (auto & drawCallback : mAfterDrawCallbackTemp)
			{
				if (std::get<bool>(drawCallback)) // append
				{
					mAfterDrawCallback.emplace_back(std::get<DrawCallback>(drawCallback));
				}
				else // delete from vector
				{
					auto & name = std::get<DrawCallback>(drawCallback).name;

					auto it = std::find_if(mAfterDrawCallback.begin(), mAfterDrawCallback.end(), [&name](const DrawCallback & callback)
					{
						return callback.name == name;
					});
					if (it != mAfterDrawCallback.end())
						mAfterDrawCallback.erase(it);
				}
			}
			mAfterDrawCallbackTemp.clear();
		}
		for (auto it = mAfterDrawCallback.begin(); it != mAfterDrawCallback.end();)
		{
			it->func(this);
			if(it->callCount > 0)
				it->callCount--;
			if(it->callCount == 0)
				it = mAfterDrawCallback.erase(it);
			else
				++it;
		}
	}

	void Renderer::ClearResource(int index)
	{
		auto & rrs = GetResource(index);
		rrs.NewFrame();
	}

	RendererFrameResource& Renderer::GetCurrentResource()
	{
		return mResource[mIndexOfResource];
	}

	int Renderer::GetNextResourceIndex() const
	{
		return (mIndexOfResource + 1) % 2;
	}

	int Renderer::GetCurrentResourceIndex() const
	{
		return mIndexOfResource;
	}

	RendererFrameResource& Renderer::GetResource(int index)
	{
		return mResource[index];
	}

	void Renderer::SetCurrentResource(int index)
	{
		mIndexOfResource = index;
	}

	RendererFrameResource& Renderer::GetCurrentRenderResource()
	{
		return mResource[(mIndexOfResource + 1) % 2];
	}

	void Renderer::Commit(RenderGroupType groupType, RenderCommand* cmd)
	{
		cmd->SetRenderIndex(GetCurrentResource().mRenderIndex++);
		GetRenderGroup(groupType).Push(cmd);
	}

	void Renderer::Commit(int groupIndex, RenderCommand* cmd)
	{
		cmd->SetRenderIndex(GetCurrentResource().mRenderIndex++);
		GetRenderGroup(groupIndex).Push(cmd);
	}

	bool Renderer::IsPresentAllResource()
	{
		if(mTag && GetCurrentRenderResource().HasRenderedFlag())
		{
			mTag = false;
			return true;
		}
		return false;
	}

	RenderQueue& Renderer::GetRenderGroup(int index)
	{
		auto & res = GetCurrentResource();
		return GetCurrentResource().GetCurrentPass().mRenderQueues[index];
	}

	RenderQueue& Renderer::GetRenderGroup(RenderGroupType groupType)
	{
		auto & res = GetCurrentResource();
		return res.GetCurrentPass().mRenderQueues[(size_t)groupType];
	}

	void Renderer::Loop()
	{
		auto & context = Context::Current();
		auto & thread = context.QueryRenderThread();
		auto & gameThread = context.QueryGameThread();
		mFrameRateLimiter.TickStand();
		bool exit = false;
		bool frameExit = false;
		mLooping = true;
		while (!exit)
		{
			if (thread.HandleMessage())
			{
				frameExit = true;
			}
			if(mFrameRateLimiter.CanExec())
			{
				auto & renderRes = GetCurrentResource();
				if (!renderRes.HasRenderedFlag())
				{
					const auto nextIndex = GetNextResourceIndex();
					SetCurrentResource(nextIndex);
					mDrawing = true;
					BeforeDraw();
					DrawCommands();
					renderRes.SetRenderedFlag();
					ClearResource(GetNextResourceIndex());
					AfterDraw();
					mDrawing = false;
					context.QueryGameThread().PostEvent(SystemEventId::LogicalFrame, {});
					DebugInscriber_Begin(mFrameRateLimiter.Tick())
					DebugInscriber_End();
				} else
				{
					if (frameExit)
						exit = true;
				}
			}
			else
			{
				// thread.Sleep(1);
			}
		}
		ClearResource(GetNextResourceIndex());
		ClearResource(GetCurrentResourceIndex());
		for (auto & resource : mResource)
		{
			for (auto pass : resource.mPass)
			{
				for (auto & renderQueue : pass->mRenderQueues)
				{
					for (auto * command : renderQueue.commands)
					{
						delete command;
					}
					renderQueue.commands.clear();
				}
			}
		}
		mLooped = true;
	}
	void Renderer::OnMessage(const Event& ev)
	{
		if(ev.GetEventGroup() == EventGroupType::System)
		{
			auto & context = Context::Current();
			switch (ev.GetSystemEventId()) {
				case SystemEventId::Size:
					context.QueryGraphicsDeviceInterface().SizeChanged(ev.GetDataInt(0), ev.GetDataInt(1));
				break; 
				case SystemEventId::Activate:
					context.QueryGraphicsDeviceInterface().CheckFullScreenForce(ev.GetData<bool>(0));
				break;
				case SystemEventId::AsynReport: break;
				case SystemEventId::AsynFinish: break;
				case SystemEventId::RenderFrame:
					// this resource can be rendered;
					GetCurrentResource().ClearRenderedFlag();
					break;
				default:;
			}
		}
	}
    #pragma region cache gpu constant buffer
	GpuBuffer Renderer::QueryUnusedGpuIndexBuffer(unsigned count)
	{
		auto & idx = GetCurrentRenderResource().mIndices;
		auto & idxused = GetCurrentRenderResource().mIndicesUsed;
		for (auto it = idx.begin(); it != idx.end(); ++it)
		{
			if (it->size == count)
			{
				auto r = *it;
				idxused.push_back(*it);
				idx.erase(it);
				return r;
			}
		}

		auto p = Context::Current().QueryGraphicsDeviceInterface().CreateIndexBuffer(count, GpuBufferType::Dynamic);
		idxused.push_back(p);
		return p;
	}

	GpuBuffer Renderer::QueryUnusedGpuVertexBuffer(unsigned size)
	{
		auto & vet = GetCurrentRenderResource().mVertices;
		auto & vetused = GetCurrentRenderResource().mVerticesUsed;
		for (auto it = vet.begin(); it != vet.end(); ++it)
		{
			if (it->size == size)
			{
				auto r = *it;
				vetused.push_back(*it);
				vet.erase(it);
				return r;
			}
		}
		auto p = Context::Current().QueryGraphicsDeviceInterface().CreateVertexBuffer(size, GpuBufferType::Dynamic);
		vetused.push_back(p);
		return p;
	}

	GpuBuffer Renderer::QueryUnusedGpuConstantBuffer(unsigned size)
	{
		auto & cb = GetCurrentRenderResource().mConstantBuffer;
		auto & cbused = GetCurrentRenderResource().mConstantBufferUsed;
		for (auto it = cb.begin(); it != cb.end(); ++it)
		{
			if (it->size == size)
			{
				auto r = *it;
				cbused.push_back(*it);
				cb.erase(it);
				return r;
			}
		}

		auto p = Context::Current().QueryGraphicsDeviceInterface().CreateConstantBuffer(size);
		cbused.push_back(p);
		return p;
	}
    #pragma endregion 

	void Renderer::SetLimitFrameRate(int frameRate)
	{
		mFrameRateLimiter.SetFrameRate(frameRate);
	}

	int Renderer::GetLimitFrameRate() const
	{
		return mFrameRateLimiter.GetStandFrameRate();
	}

	int Renderer::AppendFrameTarget(FrameBuffer * target)
	{
		auto * targetR = new RenderTargetPass();
		targetR->mTarget = target;
		GetCurrentResource().mPass.push_back(targetR);
		return GetCurrentResource().mPass.size() - 1;
	}

	int Renderer::AppendDefaultFrameTarget()
	{
		GetCurrentResource().mPass.push_back(&GetCurrentResource().mDefaultPass);
		return GetCurrentResource().mPass.size() - 1;
	}

	void Renderer::AppendAndSetDefaultFrameTarget()
	{
		SetFrameTarget(AppendDefaultFrameTarget());
	}

	void Renderer::SetDefaultFrameTarget()
	{
		auto & r = GetCurrentResource();
		for (auto i =  0 ; i < r.mPass.size(); i ++)
		{
			if(r.mPass[i] == &r.mDefaultPass)
			{
				SetFrameTarget(i);
				return;
			}
		}
		AppendAndSetDefaultFrameTarget();
	}

	void Renderer::SetFrameTarget(int index)
	{
		GetCurrentResource().mPassUsedIndex = index;
	}
	void Renderer::SetFrameTarget(FrameBuffer * target)
	{
		auto & r = GetCurrentResource();
		for (auto i = 0; i < r.mPass.size(); i++)
		{
			if (r.mPass[i]->mTarget == target)
			{
				SetFrameTarget(i);
				return;
			}
		}
	}
}
