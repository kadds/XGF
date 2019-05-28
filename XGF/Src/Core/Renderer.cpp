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
		return MakeRenderCommand(bbr, index, renderStage, 0, 0, index.GetActualCount() > 0 ? index.GetActualCount() : bbr.GetBinder(0)->GetActualCount());
	}

	DefaultRenderCommand* DefaultRenderCommand::MakeRenderCommand(const BindingBridge& bbr, const PolygonPleIndex& index, const RenderStage& renderStage, unsigned indexStart, unsigned vertexStart, unsigned drawCount)
	{
		auto & allocator = Context::Current().QueryRenderer().GetCurrentResource().GetAllocator();
		const unsigned memsize = static_cast<unsigned>(bbr.GetAllPolygonPleMemSize());
		char* vertices = allocator.NewArray<char>(memsize);
		Index* indices = allocator.NewArray<Index>(index.GetActualCount());
		auto cmd = allocator.New<DefaultRenderCommand>(vertices, bbr.GetBinder(0)->GetActualCount(), memsize, vertexStart, indices, index.GetActualCount(), indexStart, renderStage, drawCount);
		unsigned int start = 0u;
		unsigned int chunk = renderStage.GetRenderResource()->GetShader<VertexShader>()->GetStrideAllSizeAtSlot(0);
		const unsigned int* stride = renderStage.GetRenderResource()->GetShader<VertexShader>()->GetStride();
		for (int i = 0; i < bbr.Count(); i++)
		{
			bbr.GetBinder(i)->CopyTo(vertices + start, chunk);
			start += stride[i];
		}
		index.CopyTo(indices, 0);
		return cmd;
	}
	template<typename TShader>
	void BindBuffer(GDI* gdi, unsigned index, GpuBuffer& buffer, RawRenderStage& renderStage)
	{
		static_assert(false, "null shader type");
	}
	template<>
	void BindBuffer<VertexShader>(GDI* gdi, unsigned index, GpuBuffer& buffer, RawRenderStage& renderStage)
	{
		gdi->GetDeviceContext()->VSSetConstantBuffers(renderStage.GetShader<VertexShader>()->GetCBufferSlot(index), 1, &buffer.buffer);
	}
	template<>
	void BindBuffer<PixelShader>(GDI* gdi, unsigned index, GpuBuffer& buffer, RawRenderStage& renderStage)
	{
		gdi->GetDeviceContext()->PSSetConstantBuffers(renderStage.GetShader<PixelShader>()->GetCBufferSlot(index), 1, &buffer.buffer);
	}
	template<>
	void BindBuffer<GeometryShader>(GDI* gdi, unsigned index, GpuBuffer& buffer, RawRenderStage& renderStage)
	{
		gdi->GetDeviceContext()->GSSetConstantBuffers(renderStage.GetShader<GeometryShader>()->GetCBufferSlot(index), 1, &buffer.buffer);
	}

	template<typename TShader>
	void BindConstantBuffer(RawRenderStage & renderStage)
	{
		auto shader = renderStage.GetShader<TShader>();
		if (!shader) return;
		auto& context = Context::Current();
		auto& gdi = context.QueryGraphicsDeviceInterface();
		auto& renderer = context.QueryRenderer();
		auto cb = renderStage.GetConstantBuffer<TShader>();
		auto start = 0;
		for (unsigned i = 0; i < shader->GetCBufferCount(); i++)
		{
			auto buffer = renderer.GetCurrentRenderResource().QueryAvailableGpuConstantBuffer((void*)(cb + start), shader->GetCBufferSize(i));
			gdi.CopyToBuffer(buffer, (void *)(cb + start));
			BindBuffer<TShader>(&gdi, i, buffer, renderStage);
			start += shader->GetCBufferSize(i);
		}
	}


	void DefaultRenderCommand::Exec(GDI & gdi, FrameBuffer & frameBuffer)
	{
		unsigned int offset[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{ 0 };
		
		mRenderStage.BindStage();
		BindConstantBuffer<VertexShader>(mRenderStage);
		BindConstantBuffer<PixelShader>(mRenderStage);
		BindConstantBuffer<GeometryShader>(mRenderStage);

		
		auto vertexBuffer = Context::Current().QueryRenderer().GetCurrentRenderResource().QueryAvailableGpuVertexBuffer(mVertices, mVertexSize);
		auto stride = mRenderStage.GetShader<VertexShader>()->GetStrideAllSizeAtSlot(0);
		gdi.GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer.buffer, &stride, offset);
		memcpy_s(gdi.Map(vertexBuffer), mVertexSize, mVertices, mVertexSize);
		gdi.UnMap(vertexBuffer);
		if (mIndexCount > 0)
		{
			auto indexBuffer = Context::Current().QueryRenderer().GetCurrentRenderResource().QueryAvailableGpuIndexBuffer(mIndices, mIndexCount);
			gdi.GetDeviceContext()->IASetIndexBuffer(indexBuffer.buffer, sizeof(Index) > 2 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, 0);
			memcpy_s(gdi.Map(indexBuffer), sizeof(Index) * mIndexCount, mIndices, sizeof(Index) * mIndexCount);
			gdi.UnMap(indexBuffer);
			gdi.GetDeviceContext()->DrawIndexed(mDrawCount, mIndexStart, mVertexStart);
		}
		else
		{
			gdi.GetDeviceContext()->Draw(mDrawCount, mVertexStart);
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
		mPassStack.push(static_cast<unsigned>(mPass.size()) - 1);
	}

	FrameMemoryAllocator& RendererFrameResource::GetAllocator()
	{
		return mAllocator;
	}

	FrameBuffer* RendererFrameResource::Pop()
	{
		auto & r = mPass[mPassStack.top()];
		mPassStack.pop();
		return r.mTarget;
	}

	void RendererFrameResource::NewFrame()
	{
		if (mGpuBufferCachedMap)
		{
			mGpuBufferCachedMap->clear();
			mGpuBufferCachedMap->~GpuBufferCachedMap();
		}
		
		mAllocator.FreeAll();
		mGpuBufferCachedMap = mAllocator.New<GpuBufferCachedMap>();

		mOtherResource.clear();

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
		
		while (!mPassStack.empty())
			mPassStack.pop();
		
		mRenderIndex = 0;
	}

	void RendererFrameResource::Initialize()
	{
	}

	void RendererFrameResource::Shutdown()
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
		mResource[0].Initialize();
		mResource[1].Initialize();
	}

	void Renderer::Destroy()
	{
		mResource[0].Shutdown();
		mResource[1].Shutdown();
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

	FrameMemoryAllocator& Renderer::GetRenderAllocator()
	{
		return GetCurrentRenderResource().GetAllocator();
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

	FrameMemoryAllocator& Renderer::GetAllocator()
	{
		return GetCurrentResource().GetAllocator();
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
		ClearResource(GetNextResourceIndex());
		ClearResource(GetCurrentResourceIndex());
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
					context.QueryGraphicsDeviceInterface().OnReSize(ev.GetDataInt(0), ev.GetDataInt(1));
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
	void* & RendererFrameResource::GetOtherResource(size_t id)
	{
		return mOtherResource[id];
	}
	GpuBuffer RendererFrameResource::QueryUnusedGpuIndexBuffer(size_t count)
	{
		auto & idx = mIndices;
		auto & idxused = mIndicesUsed;
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

	GpuBuffer RendererFrameResource::QueryUnusedGpuVertexBuffer(size_t size)
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

	GpuBuffer RendererFrameResource::QueryUnusedGpuConstantBuffer(size_t size)
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
	GpuBuffer RendererFrameResource::QueryAvailableGpuIndexBuffer(void* ptr, size_t size)
	{
		auto buffer = FindGpuBufferCached(ptr, size);
		if (buffer.buffer != nullptr)
		{
			return buffer;
		}
		auto buf = QueryUnusedGpuIndexBuffer(size);
		SaveGpuBufferCached(ptr, size, buf);
		return buf;
	}
	GpuBuffer RendererFrameResource::QueryAvailableGpuVertexBuffer(void* ptr, size_t size)
	{
		auto buffer = FindGpuBufferCached(ptr, size);
		if (buffer.buffer != nullptr)
		{
			return buffer;
		}
		auto buf = QueryUnusedGpuVertexBuffer(size);
		SaveGpuBufferCached(ptr, size, buf);
		return buf;
	}
	GpuBuffer RendererFrameResource::QueryAvailableGpuConstantBuffer(void* ptr, size_t size)
	{
		auto buffer = FindGpuBufferCached(ptr, size);
		if (buffer.buffer != nullptr)
		{
			return buffer;
		}
		auto buf = QueryUnusedGpuConstantBuffer(size);
		SaveGpuBufferCached(ptr, size, buf);
		return buf;
	}
	GpuBuffer RendererFrameResource::FindGpuBufferCached(void* ptr, size_t size)
	{
		auto it = mGpuBufferCachedMap->find(CommonPtrKey(ptr, size));
		if (it == mGpuBufferCachedMap->end())
			return GpuBuffer(0, 0);
		return it->second;
	}
	void RendererFrameResource::SaveGpuBufferCached(void* ptr, size_t size, GpuBuffer buffer)
	{
		(*mGpuBufferCachedMap)[CommonPtrKey(ptr, size)] = buffer;
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
		auto & allocator = Context::Current().QueryRenderer().GetCurrentResource().GetAllocator();
		return allocator.New<ClearRenderCommand>(clearColor, true, clearDepth);
	}
	ClearRenderCommand * ClearRenderCommand::MakeRenderCommand(bool clearDepth)
	{
		auto& allocator = Context::Current().QueryRenderer().GetCurrentResource().GetAllocator();
		return allocator.New<ClearRenderCommand>(Color(0.f, 0.f, 0.f, 0.f), false, clearDepth);
	}

	SubRenderCommand::SubRenderCommand(std::vector<RenderCommand*>&& commands)
	{
		mCommandCount = commands.size();
		mCommands = Context::Current().QueryRenderer().GetCurrentResource().GetAllocator().NewArray<RenderCommand *>(commands.size());
		memcpy(mCommands, commands.data(), commands.size() * sizeof(RenderCommand *));
	}

	SubRenderCommand::~SubRenderCommand()
	{
	}

	void SubRenderCommand::Exec(GDI& gdi, FrameBuffer& frameBuffer)
	{
		for(int i = 0; i < mCommandCount; i++)
		{
			mCommands[i]->Exec(gdi, frameBuffer);
		}
	}

	SubRenderCommand* SubRenderCommand::MakeRenderCommand(std::vector<RenderCommand*>&& commands)
	{
		auto& allocator = Context::Current().QueryRenderer().GetCurrentResource().GetAllocator();
		return allocator.New<SubRenderCommand>(std::move(commands));
	}
	typedef std::unordered_map<BufferSharedKey, BufferSharedData, BufferSharedKeyHash, std::equal_to<BufferSharedKey>,
		FrameMemorySTLAllocator<std::pair<const BufferSharedKey, BufferSharedData>>> BufferMap;
	void BufferSharedRenderCommand::NewId(BufferSharedKey key)
	{
		auto& res = Context::Current().QueryRenderer().GetCurrentResource();
		auto*& map =
			(BufferMap * &)res.GetOtherResource(typeid(BufferSharedRenderCommand).hash_code());
		if (map == nullptr)
		{
			map = res.GetAllocator().New<BufferMap>();
		}
		auto it = map->find(key);
		if (it == map->end())
		{
			auto& bbr = key.bbr;
			auto& index = key.index;
			auto& vs = key.vs;
			const unsigned memsize = static_cast<unsigned>(bbr.GetAllPolygonPleMemSize());
			char * vertices = res.GetAllocator().NewArray<char>(memsize);
			Index * indices = res.GetAllocator().NewArray<Index>(index.GetActualCount());

			unsigned int start = 0u;
			unsigned int chunk = vs.GetStrideAllSizeAtSlot(0);
			const unsigned int* stride = vs.GetStride();
			for (int i = 0; i < bbr.Count(); i++)
			{
				bbr.GetBinder(i)->CopyTo(vertices + start, chunk);
				start += stride[i];
			}
			index.CopyTo(indices, 0);

			map->emplace(key, BufferSharedData(vertices, indices));
		}

	}
	BufferSharedData BufferSharedRenderCommand::Get(BufferSharedKey key)
	{
		auto * & map = 
			(BufferMap *&)Context::Current().QueryRenderer().GetCurrentResource().GetOtherResource(typeid(BufferSharedRenderCommand).hash_code());
		if (map == nullptr)
		{
			XGF_Error(Render, "Null BufferShadredData. Please call StartBaseRenderCommand before MakeRenderCommand");
		}
		auto it = map->find(key);
		if (it == map->end())
		{
			XGF_Error(Render, "Null BufferShadredData. Please call StartBaseRenderCommand before MakeRenderCommand");
		}
		return it->second;
	}


	void BufferSharedRenderCommand::StartBaseRenderCommand(const BindingBridge& bbr, const PolygonPleIndex& index, const VertexShader & vs)
	{
		NewId(BufferSharedKey(bbr, index, vs));
	}
	BufferSharedRenderCommand* BufferSharedRenderCommand::MakeRenderCommand(const BindingBridge& bbr, const PolygonPleIndex& index, const RenderStage& renderStage, unsigned indexStart, unsigned vertexStart, unsigned drawCount)
	{
		auto& allocator = Context::Current().QueryRenderer().GetCurrentResource().GetAllocator();
		auto data = Get(BufferSharedKey(bbr, index, *renderStage.GetRenderResource()->GetShader<VertexShader>()));
		const unsigned memsize = static_cast<unsigned>(bbr.GetAllPolygonPleMemSize());
		auto * cmd = allocator.New<BufferSharedRenderCommand>(data.mVertices, bbr.GetBinder(0)->GetActualCount(), memsize, vertexStart, data.mIndices, index.GetActualCount(), indexStart, renderStage, drawCount);
		return cmd;
	}
}
