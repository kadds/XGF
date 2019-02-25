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


	void DefaultRenderCommand::Exec(GDI & gdi, FrameBuffer & frameBuffer)
	{
		unsigned int offset[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{ 0 };

		mRenderStage.BindStage();
		if (!mRenderStage.GetConstantBuffer<VertexShader>().empty())
			for (unsigned i = 0; i < mRenderStage.GetShader<VertexShader>()->GetCBufferCount(); i++)
				BindConstantBuffer<VertexShader>(i);
		if (!mRenderStage.GetConstantBuffer<PixelShader>().empty())
			for (unsigned i = 0; i < mRenderStage.GetShader<PixelShader>()->GetCBufferCount(); i++)
				BindConstantBuffer<PixelShader>(i);
		if (!mRenderStage.GetConstantBuffer<GeometryShader>().empty())
			for (unsigned i = 0; i < mRenderStage.GetShader<GeometryShader>()->GetCBufferCount(); i++)
				BindConstantBuffer<GeometryShader>(i);


		auto vertexBuffer = Context::Current().QueryRenderer().GetCurrentRenderResource().QueryUnusedGpuVertexBuffer(mVertexSize);
		auto stride = mRenderStage.GetShader<VertexShader>()->GetStrideAllSizeAtSlot(0);
		gdi.GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer.buffer, &stride, offset);
		memcpy_s(gdi.Map(vertexBuffer), mVertexSize, mVertices, mVertexSize);
		gdi.UnMap(vertexBuffer);
		if (mIndexCount > 0)
		{
			auto indexBuffer = Context::Current().QueryRenderer().GetCurrentRenderResource().QueryUnusedGpuIndexBuffer(mIndexCount);
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

	void RenderTargetPass::Clear()
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


	unsigned RendererFrameResource::NextRenderIndex()
	{
		return mRenderIndex++;
	}

	std::vector<RenderTargetPass>& RendererFrameResource::GetAllPasses()
	{
		return mPass;
	}

	RenderTargetPass& RendererFrameResource::GetCurrentPass()
	{
		XGF_ASSERT(!mPassStack.empty());
		return mPass[mPassStack.top()];
	}

	void RendererFrameResource::Push(FrameBuffer* frameBuffer)
	{
		mPass.emplace_back(frameBuffer);
		mPassStack.push(mPass.size() - 1);
	}

	FrameBuffer* RendererFrameResource::Pop()
	{
		auto & r = mPass[mPassStack.top()];
		mPassStack.pop();
		return r.mTarget;
	}

	void RendererFrameResource::NewFrame()
	{
		mVertices.insert(mVertices.end(), mVerticesUsed.begin(), mVerticesUsed.end());
		mVerticesUsed.clear();

		mIndices.insert(mIndices.end(), mIndicesUsed.begin(), mIndicesUsed.end());
		mIndicesUsed.clear();

		mConstantBuffer.insert(mConstantBuffer.end(), mConstantBufferUsed.begin(), mConstantBufferUsed.end());
		mConstantBufferUsed.clear();
		for(auto & it : mPass)
		{
			it.Clear();
		}
		mPass.clear();
		
		while (mPassStack.empty())
			mPassStack.pop();
		
		mRenderIndex = 0;
	}

	void RendererFrameResource::ClearAll()
	{
		for (auto & pass : mPass)
		{
			pass.Clear();
		}
		mPass.clear();
		for (GpuBuffer & buffer : mVertices)
		{
			buffer.buffer->Release();
		}
		for (GpuBuffer & buffer : mVerticesUsed)
		{
			buffer.buffer->Release();
		}
		for (GpuBuffer& buffer : mIndices)
		{
			buffer.buffer->Release();
		}
		for (GpuBuffer& buffer : mIndicesUsed)
		{
			buffer.buffer->Release();
		}
		for (GpuBuffer & buffer : mConstantBuffer)
		{
			buffer.buffer->Release();
		}
		for (GpuBuffer & buffer : mConstantBufferUsed)
		{
			buffer.buffer->Release();
		}
	}

	void Renderer::ClearDepthStencilBuffer(RenderGroupType groupType)
	{
		Commit(groupType, ClearRenderCommand::MakeRenderCommand(true));
	}

	void Renderer::Clear(const Color& color, RenderGroupType groupType)
	{
		Commit(groupType, ClearRenderCommand::MakeRenderCommand(color, true));
	}

	void Renderer::ClearColor(const Color& color, RenderGroupType groupType)
	{
		Commit(groupType, ClearRenderCommand::MakeRenderCommand(color));
	}

	void Renderer::ClearDepthStencilBuffer(RenderGroupRawType groupType)
	{
		Commit(groupType, ClearRenderCommand::MakeRenderCommand(true));
	}

	void Renderer::Clear(const Color& color, RenderGroupRawType groupType)
	{
		Commit(groupType, ClearRenderCommand::MakeRenderCommand(color, true));
	}

	void Renderer::ClearColor(const Color& color, RenderGroupRawType groupType)
	{
		Commit(groupType, ClearRenderCommand::MakeRenderCommand(color, true));
	}

	void Renderer::Create()
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		gdi.Create();
		Context::Current().QueryRenderThread().SetCallBackFunc(std::bind(&Renderer::OnMessage, this, std::placeholders::_1));
		mIndexOfResource = 0;
		GetCurrentRenderResource().SetRenderedFlag();
		GetCurrentResource().SetRenderedFlag();
		mLooping = false;
		mLooped = false;
		mDrawing = false;
	}

	void Renderer::Destroy()
	{
		for (auto & resource : mResource)
		{
			resource.ClearAll();
		}

		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		gdi.Destroy();
	}


	FrameBuffer * Renderer::GetCurrentFrameTarget()
	{
		return GetCurrentResource().GetCurrentPass().mTarget;
	}

	void Renderer::PushFrameTarget(FrameBuffer* target)
	{
		GetCurrentResource().Push(target);
	}

	void Renderer::PushDefaultFrameTarget()
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		GetCurrentResource().Push(gdi.GetDisplayFrameBuffer());
	}

	FrameBuffer* Renderer::PopFrameTarget()
	{
		auto * fb = GetCurrentResource().Pop();
		return fb;
	}

	FrameBuffer* Renderer::PopNewFrameTarget()
	{
		auto * fb = GetCurrentResource().Pop();
		auto * newFrame = GetCurrentResource().Pop();
		PushFrameTarget(newFrame);
		return fb;
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
		auto & gdi = context.QueryGraphicsDeviceInterface();
		for (auto & pass : GetCurrentRenderResource().GetAllPasses())
		{
			bool binded = false;
			for (auto & renderQueue : pass.mRenderQueues)
			{
				renderQueue.Reorder();
				if(!binded && !renderQueue.commands.empty())
				{
					pass.mTarget->Bind();
					binded = true;
				}
				for (auto & command : renderQueue.commands)
				{
					command->Exec(gdi, *pass.mTarget);
				}
			}
		}
		gdi.Present(false);
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
		cmd->SetRenderIndex(GetCurrentResource().NextRenderIndex());
		GetRenderGroup(groupType).Push(cmd);
	}

	void Renderer::Commit(RenderGroupRawType groupIndex, RenderCommand* cmd)
	{
		cmd->SetRenderIndex(GetCurrentResource().NextRenderIndex());
		GetRenderGroup(groupIndex).Push(cmd);
	}

	RenderQueue& Renderer::GetRenderGroup(RenderGroupRawType index)
	{
		auto & res = GetCurrentResource();
		return res.GetCurrentPass().mRenderQueues[index];
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
			for (auto & pass : resource.GetAllPasses())
			{
				pass.Clear();
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
	GpuBuffer RendererFrameResource::QueryUnusedGpuIndexBuffer(unsigned count)
	{
		auto & idx = mVertices;
		auto & idxused = mVerticesUsed;
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

	GpuBuffer RendererFrameResource::QueryUnusedGpuVertexBuffer(unsigned size)
	{
		auto & vet = mVertices;
		auto & vetused = mVerticesUsed;
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

	GpuBuffer RendererFrameResource::QueryUnusedGpuConstantBuffer(unsigned size)
	{
		auto & cb = mConstantBuffer;
		auto & cbused = mConstantBufferUsed;
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

	void ClearRenderCommand::Exec(GDI & gdi, FrameBuffer & frameBuffer)
	{
		if(mClearTarget)
		{
			frameBuffer.Clear(mClearColor);
		}
		if(mClearDepth)
		{
			frameBuffer.ClearDepthStencilBuffer();
		}
	}

	ClearRenderCommand * ClearRenderCommand::MakeRenderCommand(const Color & clearColor, bool clearDepth)
	{
		return new ClearRenderCommand(clearColor, true, clearDepth);
	}
	ClearRenderCommand * ClearRenderCommand::MakeRenderCommand(bool clearDepth)
	{
		return new ClearRenderCommand(Color(0.f, 0.f, 0.f, 0.f), false, clearDepth);
	}

	SubRenderCommand::~SubRenderCommand()
	{
		for (auto it : mCommands)
		{
			delete it;
		}
	}

	void SubRenderCommand::Exec(GDI& gdi, FrameBuffer& frameBuffer)
	{
		for(auto it : mCommands)
		{
			it->Exec(gdi, frameBuffer);
		}
	}

	SubRenderCommand* SubRenderCommand::MakeRenderCommand(std::vector<RenderCommand*>&& commands)
	{
		return new SubRenderCommand(std::move(commands));
	}
}
