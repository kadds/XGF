#include "../../Include/Batch.hpp"
#include "../../Include/Logger.hpp"
#include <DirectXMath.h>
#include "../../Include/DebugInscriber.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Texture.hpp"
#include "../../Include/Shader.hpp"
#include "../../Include/Polygon.hpp"

namespace XGF
{
	int Batch::mClientWidth;
	int Batch::mClientHeight;
	unsigned int Batch::mMaxPreRenderFrameCount = 3;

	void Batch::DrawPolygon(const BindingBridge & bbridge)
	{
		XGF_ASSERT(mIsBegin);
		//Map Buffer
		if (!mIsMap) {
			Map(false);
			mIsMap = true;
		}
		else if (mMaxVertices - mPosInVertices <  bbridge.GetBinder(0)->Count())
		{
			XGF_Warn(Render, "Out of Range In Vertex. You must set a larger buffer size.");
			ResizeVertexBuffer(bbridge.GetBinder(0)->Count() + mPosInVertices);
			return;
		}
		auto vs = mShaderStage.GetVSShader();
		UINT nsizepos = 0;
		//copy 顶点数据
		unsigned int slotPosition = 0;
		unsigned int count = vs->GetSlotCount();
		unsigned int * elementSlotInfo = vs->GetSlotElementStartPositionArray();
		unsigned int * stride = vs->GetStride();

		unsigned int slotStride = vs->GetStrideAllSizeAtSlot(slotPosition);
		for (unsigned int i = 0; i < bbridge.Count(); i++)
		{
			if (slotPosition < count && (slotPosition == count - 1 ? false : elementSlotInfo[slotPosition + 1] <= i)) {
				slotPosition ++; // new slot
				nsizepos = 0;
				slotStride = vs->GetStrideAllSizeAtSlot(slotPosition);
			}
			
			bbridge.GetBinder(i)->CopyTo(mVertexData[slotPosition] + slotStride * (mPosInVertices + mLastFrameVBStart) + nsizepos, slotStride);
			nsizepos += stride[i];
		}
		mPosInVertices += bbridge.GetBinder(0)->Count();
		DebugInscriber_CallAPolygon();
	}

	void Batch::DrawPolygon(std::shared_ptr<PolygonPleIndex> pindex, const BindingBridge & bbridge)
	{
		XGF_ASSERT(mIsBegin);
		if (mMaxIndexCount - mPosInIndices < pindex->mCount)
		{
			XGF_Warn(Render, "Out of Range In Index. You must set a larger buffer size.");
			ResizeIndexBuffer(pindex->mCount + mPosInIndices);
			return;
		}
		DrawPolygon(bbridge);
		pindex->CopyTo(mIndexData + mPosInIndices + mLastFrameIBStart, mPosInVertices - bbridge.GetBinder(0)->Count() + mLastFrameVBStart);
		mPosInIndices += pindex->mCount;
	}

	void Batch::Flush()
	{
		if (mIsBegin)
		{
			EndWithoutFrame();
			mBeforeVertices = mPosInVertices;
			mPosBeforeIndices = mPosInIndices;
			mIsBegin = true;
		}
	}

	void Batch::Initialize(GDI * gdi, Shaders shaders, int MaxVertices, int MaxIndexCount, TopologyMode tm)
	{
		XGF_ASSERT(MaxVertices > 0);
		mGDI = gdi;
		mMaxVertices = MaxVertices;
		mMaxIndexCount = MaxIndexCount;
		mShaderStage.Initialize(shaders.vs, shaders.ps, shaders.gs);
		mTopologyMode = tm;
		//初始化
		mLastFrameVBStart = 0;
		mLastFrameIBStart = 0;
		mFramePosition = 0;
		mIsBegin = false;
		mIsMap = false;
		mNewIndexCount = MaxIndexCount;
		mNewVertexCount = MaxVertices;
		mIndexBuffer = nullptr;
		if (MaxIndexCount <= 0)
		{
			mUsingIndex = false;
		}
		else
			mUsingIndex = true;
		mUsingZBuffer = true;
		if (mShaderStage.GetGSShader() != nullptr && mShaderStage.GetGSShader()->IsStreamOut())
		{
			mUsingIndex = false;
			mUsingZBuffer = false;
		}
		CreateIndexBuffer();
		if (MaxVertices <= 0)
			return;
		
		auto vs = mShaderStage.GetVSShader();
		unsigned int slotCount = vs->GetSlotCount();
		for (unsigned int i = 0; i < slotCount; i++)
		{
			mVertexBuffers.push_back(CreateVertexBuffer(vs->GetStrideAllSizeAtSlot(i)));
			mVertexData.push_back(nullptr);
		}
		
		mShaderStage.SetOnFlushListener(std::bind(&Batch::Flush, this));
	}
	void Batch::Begin()
	{
		if (mIsBegin)
			return;//TODO::ERROR
		mIsBegin = true;
		mPosInIndices = 0;
		mPosInVertices = 0;
		mPosBeforeIndices = 0;
		mBeforeVertices = 0;
		CheckAndResizeBuffers();
	}
	void Batch::End()
	{
		EndWithoutFrame();
		DebugInscriber_CallABatch(mPosInIndices, mPosInVertices);

		mLastFrameVBStart += mPosInVertices;
		mLastFrameIBStart += mPosInIndices;
		mFramePosition++;
		if (mFramePosition >= mMaxPreRenderFrameCount)
		{
			mFramePosition = 0;
			mLastFrameVBStart = 0;
			mLastFrameIBStart = 0;
		}

	}

	void Batch::ChangeTopologyMode(TopologyMode tm)
	{
		if (mTopologyMode != tm)
		{
			Flush();
			mTopologyMode = tm;
		}
	}
	
	TopologyMode Batch::GetTopologyMode()
	{
		return mTopologyMode;
	}

	/*void Batch::End(ID3D11Buffer * c, int count)
	{
		mVertexBuffer = c;
		EndWithoutFrame(true);
		mVertexBuffer = nullptr;
	}*/

	void Batch::StepVertices(int count)
	{
		mPosInVertices += count;
	}

	Batch::Batch()
	{
	}

	void Batch::Shutdown()
	{
		if (mUsingIndex)
			mIndexBuffer->Release();
		for each (auto it in mVertexBuffers)
		{
			it->Release();
		}
		mShaderStage.Shutdown();
	}

	void Batch::Map(bool discard)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		int i = 0;
		for each (auto it in mVertexBuffers)
		{
			mGDI->GetDeviceContext()->Map(it, 0, discard ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
			mVertexData[i++] = static_cast<char*>(mappedResource.pData);
		}

		if (mUsingIndex)
		{
			mGDI->GetDeviceContext()->Map(mIndexBuffer, 0, discard ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
			mIndexData = static_cast<Index *>(mappedResource.pData);
		}
	}

	void Batch::UnMap()
	{
		for each (auto it in mVertexBuffers)
		{
			mGDI->GetDeviceContext()->Unmap(it, 0);
		}
		
		if (mUsingIndex)
		{
			mGDI->GetDeviceContext()->Unmap(mIndexBuffer, 0);
		}

	}
	void Batch::PrepareForRender()
	{
		auto gdi = mGDI;
		unsigned int offset[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{ 0 };
		
		ID3D11Buffer *mbuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		memset(mbuffer, 0, sizeof(mbuffer));
		mShaderStage.BindStage();
		auto vs = mShaderStage.GetVSShader();
		vs->SetInputLayout();
		
		if (mShaderStage.GetGSShader() != nullptr) //gs
		{
			auto gs = mShaderStage.GetGSShader();
			if (gs->IsStreamOut())
			{
				auto buf = mShaderStage.GetGSShader()->GetStreamOutBuffer();
				gdi->GetDeviceContext()->SOSetTargets(1, &buf, offset);
			}
		}
		
		for (unsigned int i = 0; i < vs->GetSlotCount(); i++) {
			unsigned int stride = vs->GetStrideAllSizeAtSlot(i);
			gdi->GetDeviceContext()->IASetVertexBuffers(i, 1, &mVertexBuffers[i], &stride, offset);
		}
		if (mMaxVertices > 0)
			gdi->GetDeviceContext()->IASetVertexBuffers(vs->GetSlotCount(), D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - vs->GetSlotCount(), mbuffer, offset, offset);
		
		if (mUsingIndex)
			gdi->GetDeviceContext()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		gdi->GetDeviceContext()->IASetPrimitiveTopology(mTopologyMode);
		
	}

	void Batch::ResizeIndexBuffer(int newSize)
	{
		mNewIndexCount = mNewIndexCount > newSize ? mNewIndexCount : newSize;
	}

	void Batch::ResizeVertexBuffer(int newSize)
	{
		mNewVertexCount = mNewVertexCount > newSize ? mNewVertexCount : newSize;
	}

	void Batch::CheckAndResizeBuffers()
	{
		if(mNewVertexCount != mMaxVertices)
		{
			for(auto it : mVertexBuffers)
			{
				it->Release();
			}
			mVertexBuffers.clear();
			mVertexData.clear();
			mMaxVertices = mNewVertexCount;
			auto vs = mShaderStage.GetVSShader();
			unsigned int slotCount = vs->GetSlotCount();
			for (unsigned int i = 0; i < slotCount; i++)
			{
				mVertexBuffers.push_back(CreateVertexBuffer(vs->GetStrideAllSizeAtSlot(i)));
				mVertexData.push_back(nullptr);
			}
		}
		if(mNewIndexCount != mMaxIndexCount)
		{
			mMaxIndexCount = mNewIndexCount;
			CreateIndexBuffer();
		}
	}

	ID3D11Buffer * Batch::CreateVertexBuffer(unsigned len)
	{
		ID3D11Buffer * buffer;
		D3D11_BUFFER_DESC vertexDesc;
		char *vertices = new char[mMaxVertices * len * mMaxPreRenderFrameCount];
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = len * mMaxVertices * mMaxPreRenderFrameCount;
		vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = vertices;
		XGF_Error_Check(Render, mGDI->GetDevice()->CreateBuffer(&vertexDesc, &resourceData, &buffer), "CreateVertexBuffer Error");
		PutDebugString((buffer));
		delete[] vertices;
		return buffer;
	}

	void Batch::EndWithoutFrame(bool drawAuto)
	{
		if (!mIsBegin)
			return;//TODO::ERROR
		mIsBegin = false;

		if (mIsMap) {
			UnMap();
			mIsMap = false;
		}
		if (mPosInIndices == 0 && mUsingIndex || !drawAuto && mPosInVertices - mBeforeVertices <= 0 && !mUsingIndex)
			return;
		PrepareForRender();
		if (mShaderStage.GetGSShader() != nullptr && mShaderStage.GetGSShader()->IsStreamOut())
		{
			mGDI->GetDeviceContext()->Draw(mPosInVertices - mBeforeVertices, mLastFrameVBStart + mBeforeVertices);
			mShaderStage.GetGSShader()->Swap();
		}
		else
		{
			if (!drawAuto)
				if (mUsingIndex)
					mGDI->GetDeviceContext()->DrawIndexed(mPosInIndices - mPosBeforeIndices, mLastFrameIBStart + mPosBeforeIndices, 0);
				else
					mGDI->GetDeviceContext()->Draw(mPosInVertices - mBeforeVertices, mLastFrameVBStart + mBeforeVertices);
			else
				mGDI->GetDeviceContext()->DrawAuto();
		}
		ID3D11Buffer *b = nullptr;
		UINT offset[4] = { 0 };
		mGDI->GetDeviceContext()->SOSetTargets(1, &b, offset);
		mShaderStage.UnBindStage();
	}

	void Batch::CreateIndexBuffer()
	{
		/*UINT index[]{
		1, 2, 0,
		0, 3, 1
		};*/
		if (!mUsingIndex) return;
		if (mIndexBuffer) mIndexBuffer->Release();
		auto *indexList = new Index[mMaxIndexCount * mMaxPreRenderFrameCount];
		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_DYNAMIC;
		ibd.ByteWidth = sizeof(Index) *(mMaxIndexCount * mMaxPreRenderFrameCount);
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		ZeroMemory(&iinitData, sizeof(iinitData));
		iinitData.pSysMem = indexList;
		XGF_Error_Check(Render, mGDI->GetDevice()->CreateBuffer(&ibd, &iinitData, &mIndexBuffer), "CreateIndexBuffer Error");
		//mIndexBuffer = CreateBuffer(mGDI, D3D11_BIND_INDEX_BUFFER, 0, sizeof(index) *(mMaxIndexCount * mMaxPreRenderFrameCount), 0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		PutDebugString(mIndexBuffer);
		delete[] indexList;
	}

}
