#include "./../../Include/Renderer.hpp"
#include "../../Include/Asyn.hpp"
#include "../../Include/Polygon.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/DebugInscriber.hpp"

namespace XGF
{

	#pragma region Bind Constant Buffer
	void DefaultRenderCommand::BindVSConstantBuffer(unsigned index)
	{
		auto & context = Context::Current();
		auto & gdi = context.QueryGraphicsDeviceInterface();
		auto & cb = mShaderStage.GetVSConstantBuffer();
		auto buffer = context.QueryRenderer().QueryUnusedGpuConstantBuffer(mShaderStage.GetVSShader()->GetCBufferSize(index));
		gdi.CopyToBuffer(buffer, cb[index].GetBufferPoint());
		gdi.GetDeviceContext()->VSSetConstantBuffers(mShaderStage.GetVSShader()->GetCBufferSlot(index), 1, &buffer.buffer);

	}

	void DefaultRenderCommand::BindPSConstantBuffer(unsigned index)
 	{
		auto & context = Context::Current();
		auto & gdi = context.QueryGraphicsDeviceInterface();
		auto & cb = mShaderStage.GetPSConstantBuffer();
		auto buffer = context.QueryRenderer().QueryUnusedGpuConstantBuffer(mShaderStage.GetPSShader()->GetCBufferSize(index));
		gdi.CopyToBuffer(buffer, cb[index].GetBufferPoint());
		gdi.GetDeviceContext()->PSSetConstantBuffers(mShaderStage.GetPSShader()->GetCBufferSlot(index), 1, &buffer.buffer);

	}

	void DefaultRenderCommand::BindGSConstantBuffer(unsigned index)
	{
		auto & context = Context::Current();
		auto & gdi = context.QueryGraphicsDeviceInterface();
		auto & cb = mShaderStage.GetGSConstantBuffer();
		auto buffer = context.QueryRenderer().QueryUnusedGpuConstantBuffer(mShaderStage.GetGSShader()->GetCBufferSize(index));
		gdi.CopyToBuffer(buffer, cb[index].GetBufferPoint());
		gdi.GetDeviceContext()->VSSetConstantBuffers(mShaderStage.GetGSShader()->GetCBufferSlot(index), 1, &buffer.buffer);
	}
	#pragma endregion 

	DefaultRenderCommand* DefaultRenderCommand::MakeRenderCommand(const BindingBridge& bbr,
		const PolygonPleIndex& index,
		const ShaderStage& shaderStage)
	{
		const size_t memsize = bbr.GetAllPolygonPleMemSize();
		char* vertices = new char[memsize];
		std::vector<float> vertices1(memsize / sizeof(float));
		auto indices = new Index[index.GetActualCount()];
		auto cmd = new DefaultRenderCommand(vertices, bbr.GetBinder(0)->GetActualCount(), memsize, indices, index.GetActualCount());
		unsigned int start = 0u;
		unsigned int chunk = shaderStage.GetVSShader()->GetStrideAllSizeAtSlot(0);
		const unsigned int * stride = shaderStage.GetVSShader()->GetStride();
 		for (int i = 0; i < bbr.Count(); i++)
		{
			bbr.GetBinder(i)->CopyTo(vertices + start, chunk);
			bbr.GetBinder(i)->CopyTo((char*)( vertices1.data()) + start, chunk);
			start += stride[i];
		}
		cmd->mShaderStage = shaderStage;
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
		mShaderStage.BindStage();
		if (!mShaderStage.GetVSConstantBuffer().empty())
			for (int i = 0; i < mShaderStage.GetVSShader()->GetCBufferCount(); i++)
				BindVSConstantBuffer(i);
		if (!mShaderStage.GetPSConstantBuffer().empty())
			for (int i = 0; i < mShaderStage.GetPSShader()->GetCBufferCount(); i++)
				BindPSConstantBuffer(i);
		if (!mShaderStage.GetGSConstantBuffer().empty())
			for (int i = 0; i < mShaderStage.GetGSShader()->GetCBufferCount(); i++)
				BindGSConstantBuffer(i);


		auto vertexBuffer = Context::Current().QueryRenderer().QueryUnusedGpuVertexBuffer(mVertexSize);
		auto stride = mShaderStage.GetVSShader()->GetStrideAllSizeAtSlot(0);
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

		mShaderStage.UnBindStage();
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

	RenderTargetPass& RenderResource::GetCurrentPass()
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

	void RenderResource::NewFrame()
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
		GetCurrentResource().mDefaultPass.mTarget = gdi.GetDisplayRenderTarget();
		GetCurrentRenderResource().mDefaultPass.mTarget = gdi.GetDisplayRenderTarget();

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

	void Renderer::AppendAndSetRenderTarget(RenderTarget* target)
	{
		SetRenderTarget(AppendRenderTarget(target));
	}

	void Renderer::AppendBeforeDrawCallback(const std::string & name, RendererDrawCallback callback)
	{
		std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
		mBeforeDrawCallbackTemp.emplace_back(callback, name, true);
	}

	void Renderer::AppendAfterDrawCallback(const std::string & name, RendererDrawCallback callback)
	{
		std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
		mAfterDrawCallbackTemp.emplace_back(callback, name, true);
	}

	void Renderer::RemoveBeforeDrawCallback(const std::string & name)
	{
		std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
		mBeforeDrawCallbackTemp.emplace_back(nullptr, name, false);
	}

	void Renderer::RemoveAfterDrawCallback(const std::string & name)
	{
		std::lock_guard<std::recursive_mutex> lock(mDrawMutex);
		mAfterDrawCallbackTemp.emplace_back(nullptr, name, false);
	}

	void Renderer::DrawCommands()
	{
		auto & context = Context::Current();
		for (auto pass : GetCurrentRenderResource().mPass)
		{
			context.QueryGraphicsDeviceInterface().SetRenderTarget(pass->mTarget);
			pass->mTarget->Clear(pass->mClearColor);
			for (auto & renderQueue : pass->mRenderQueues)
			{
				renderQueue.Reorder();
				for (auto & command : renderQueue.commands)
				{
					command->Exec();
					delete command;
				}
				if (!renderQueue.commands.empty())
				{
					pass->mTarget->ClearDepthStencilBuffer();
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
					mBeforeDrawCallback.emplace_back(std::get<RendererDrawCallback>(drawCallback), std::get<std::string>(drawCallback));
				}
				else // delete from vector
				{
					auto & name = std::get<std::string>(drawCallback);
					
					auto it = std::find_if(mBeforeDrawCallback.begin(), mBeforeDrawCallback.end(), [&name](std::pair<RendererDrawCallback, std::string> ele)
					{
						return ele.second == name;
					});
					if (it != mBeforeDrawCallback.end())
						mBeforeDrawCallback.erase(it);
				}
			}
		}
		for(int i = 0; i < mBeforeDrawCallback.size(); i++)
		{
			mBeforeDrawCallback[i].first(this);
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
					mAfterDrawCallback.emplace_back(std::get<RendererDrawCallback>(drawCallback), std::get<std::string>(drawCallback));
				}
				else // delete from vector
				{
					auto & name = std::get<std::string>(drawCallback);

					auto it = std::find_if(mAfterDrawCallback.begin(), mAfterDrawCallback.end(), [&name](std::pair<RendererDrawCallback, std::string> ele)
					{
						return ele.second == name;
					});
					if (it != mAfterDrawCallback.end())
						mAfterDrawCallback.erase(it);
				}
			}
		}
		for (int i = 0; i < mAfterDrawCallback.size(); i++)
		{
			mAfterDrawCallback[i].first(this);
		}
	}

	void Renderer::ClearResource(int index)
	{
		auto & rrs = GetResource(index);
		rrs.NewFrame();
	}

	RenderResource& Renderer::GetCurrentResource()
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

	RenderResource& Renderer::GetResource(int index)
	{
		return mResource[index];
	}

	void Renderer::SetCurrentResource(int index)
	{
		mIndexOfResource = index;
	}

	RenderResource& Renderer::GetCurrentRenderResource()
	{
		return mResource[(mIndexOfResource + 1) % 2];
	}

	void Renderer::Commit(RenderGroupType groupType, RenderCommand* cmd)
	{
		GetRenderGroup(groupType).Push(cmd);
	}

	void Renderer::Commit(int groupIndex, RenderCommand* cmd)
	{
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
		mFrameRateLimiter.TickStand();
		bool exit = false;
		bool frameExit = false;
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
					BeforeDraw();
					DrawCommands();
					renderRes.SetRenderedFlag();
					AfterDraw();
					const auto nextIndex = GetNextResourceIndex();
					ClearResource(nextIndex);
					SetCurrentResource(nextIndex);
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

	int Renderer::AppendRenderTarget(RenderTarget* target)
	{
		auto * targetR = new RenderTargetPass();
		targetR->mTarget = target;
		GetCurrentResource().mPass.push_back(targetR);
		return GetCurrentResource().mPass.size() - 1;
	}

	int Renderer::AppendDefaultRenderTarget()
	{
		GetCurrentResource().mPass.push_back(&GetCurrentResource().mDefaultPass);
		return GetCurrentResource().mPass.size() - 1;
	}

	void Renderer::AppendAndSetDefaultRenderTarget()
	{
		SetRenderTarget(AppendDefaultRenderTarget());
	}

	void Renderer::SetDefaultRenderTarget()
	{
		auto & r = GetCurrentResource();
		for (auto i =  0 ; i < r.mPass.size(); i ++)
		{
			if(r.mPass[i] == &r.mDefaultPass)
			{
				SetRenderTarget(i);
				return;
			}
		}
		AppendAndSetDefaultRenderTarget();
	}

	void Renderer::SetRenderTarget(int index)
	{
		GetCurrentResource().mPassUsedIndex = index;
	}
}
