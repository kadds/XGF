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

	void Renderer::Clear(const Color& color)
	{
		GetCurrentResource().mClearColor = color;
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
	}

	void Renderer::Destroy()
	{
		for (auto & resource : mResource)
		{
			resource.mRenderQueues.clear();
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

	void Renderer::DrawCommands()
	{
		auto & context = Context::Current();
		context.QueryGraphicsDeviceInterface().Clear(GetCurrentRenderResource().mClearColor);
		for (auto & renderQueue : GetCurrentRenderResource().mRenderQueues)
		{
			renderQueue.Reorder();
			for (auto & command : renderQueue.commands)
			{
				command->Exec();
				delete command;
			}
			if(!renderQueue.commands.empty())
			{
				context.QueryGraphicsDeviceInterface().ClearDepthStencilBuffer();
			}
			renderQueue.commands.clear();
		}
		context.QueryGraphicsDeviceInterface().Present(false);
	}

	void Renderer::ClearResource(int index)
	{
		auto & rrs = GetResource(index);
		rrs.mVertices.insert(rrs.mVertices.end(), rrs.mVerticesUsed.begin(), rrs.mVerticesUsed.end());
		rrs.mVerticesUsed.clear();

		rrs.mIndices.insert(rrs.mIndices.end(), rrs.mIndicesUsed.begin(), rrs.mIndicesUsed.end());
		rrs.mIndicesUsed.clear();

		rrs.mConstantBuffer.insert(rrs.mConstantBuffer.end(), rrs.mConstantBufferUsed.begin(), rrs.mConstantBufferUsed.end());
		rrs.mConstantBufferUsed.clear();

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
		return GetCurrentResource().mRenderQueues[index];
	}

	RenderQueue& Renderer::GetRenderGroup(RenderGroupType groupType)
	{
		return GetCurrentResource().mRenderQueues[(size_t)groupType];
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
					DrawCommands();
					renderRes.SetRenderedFlag();
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
			for (auto & renderQueue : resource.mRenderQueues)
			{
				for (auto * command : renderQueue.commands)
				{
					delete command;
				}
				renderQueue.commands.clear();
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
}
