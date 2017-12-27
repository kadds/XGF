#include "../../Include/Batch.hpp"
#include "../../Include/Log.hpp"
#include <DirectXMath.h>
#include "../../Include/DebugInscriber.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Texture.hpp"
#include "../../Include/Shader.hpp"
#include "../../Include/Polygon.hpp"
#include <algorithm>
namespace XGF
{
	int Batch::mClientWidth;
	int Batch::mClientHeight;
	unsigned int Batch::mMaxPreRenderFrameCount = 3;

	void Batch::DrawPolygon(const BindingBridge & bbridge)
	{
		//Map Buffer
		if (!mIsMap) {
			Map(false);
			mIsMap = true;
		}
		else if (mMaxVertices - mPosInVertices <  bbridge.GetBinder(0)->mCount)
		{
			XGF_ReportWarn("Out of Range In Vertex. You must set a larger buffer size.", "");
			return;//TODO::ERROR
		}
		auto vs = mShaderStage.GetVSShader();
		UINT nsizepos = 0;
		//copy 顶点数据
		unsigned int tisk = vs->GetStrideAllSize();
		for (int i = 0; i < bbridge.GetBinderCount(); i++)
		{
			bbridge.GetBinder(i)->CopyTo(mVertexData + tisk * (mPosInVertices + mLastFrameVBStart) + nsizepos, tisk);
			nsizepos += vs->GetStride()[i];
		}
		mPosInVertices += bbridge.GetBinder(0)->mCount;
		DebugInscriber_CallAPolygon();
	}

	void Batch::DrawPolygon(const PolygonPleIndex & pindex, const BindingBridge & bbridge)
	{
		if (mMaxIndexCount - mPosInIndices < pindex.mCount)
		{
			XGF_ReportWarn("Out of Range In Index. You must set a larger buffer size.", "");
			return;//TODO::ERROR
		}
		DrawPolygon(bbridge);
		pindex.CopyTo(mIndexData + mPosInIndices + mLastFrameIBStart, mPosInVertices - bbridge.GetBinder(0)->mCount + mLastFrameVBStart);
		mPosInIndices += pindex.mCount;
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
		mVertexBuffer = nullptr;
		mVertexData = nullptr;
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
			mVertexData = nullptr;
		}
		CreateIndexBuffer();
		if (MaxVertices <= 0)
			return;
		auto vs = mShaderStage.GetVSShader();
		CreateVertexBuffer(vs->GetStrideAllSize(), &mVertexBuffer);

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

	void Batch::End(ID3D11Buffer * c, int count)
	{
		mVertexBuffer = c;
		EndWithoutFrame(true);
		mVertexBuffer = nullptr;
	}

	void Batch::StepVetices(int count)
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
			
		if(mVertexBuffer != nullptr)
			mVertexBuffer->Release();
		mShaderStage.Shutdown();
	}

	void Batch::Map(bool discard)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		mGDI->GetDeviceContext()->Map(mVertexBuffer, 0, discard ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
		mVertexData = static_cast<char*>(mappedResource.pData);
		
		if (mUsingIndex)
		{
			mGDI->GetDeviceContext()->Map(mIndexBuffer, 0, discard ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
			mIndexData = static_cast<index *>(mappedResource.pData);
		}
	}

	void Batch::UnMap()
	{
		mGDI->GetDeviceContext()->Unmap(mVertexBuffer, 0);
		if (mUsingIndex)
		{
			mGDI->GetDeviceContext()->Unmap(mIndexBuffer, 0);
		}

	}
	void Batch::PrepareForRender()
	{
		auto gdi = mGDI;
		unsigned int offset[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{ 0 };
		unsigned int stride[] = { mShaderStage.GetVSShader()->GetStrideAllSize() };
		ID3D11Buffer *mbuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		memset(mbuffer, 0, sizeof(mbuffer));
		mShaderStage.BindStage();
		mShaderStage.GetVSShader()->SetInputLayout();
		
		if (mMaxVertices > 0)
			gdi->GetDeviceContext()->IASetVertexBuffers(1, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1, mbuffer, offset, offset);
		if (mShaderStage.GetGSShader() != nullptr) //gs
		{
			auto gs = mShaderStage.GetGSShader();
			if (gs->IsStreamOut())
			{
				auto buf = mShaderStage.GetGSShader()->GetStreamOutBuffer();
				gdi->GetDeviceContext()->SOSetTargets(1, &buf, offset);
			}
		}
		gdi->GetDeviceContext()->IASetVertexBuffers(0, 1, &mVertexBuffer, stride, offset);
		
		if (mUsingIndex)
			gdi->GetDeviceContext()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		gdi->GetDeviceContext()->IASetPrimitiveTopology(mTopologyMode);
		
	}

	void Batch::CreateVertexBuffer(unsigned len, ID3D11Buffer ** buffer)
	{
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
		XGF_Error_Check(mGDI->GetDevice()->CreateBuffer(&vertexDesc, &resourceData, buffer), "CreateVertexBuffer Error");
		PutDebugString((*buffer));
		delete[] vertices;
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
		index *indexList = new index[mMaxIndexCount * mMaxPreRenderFrameCount];
		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_DYNAMIC;
		ibd.ByteWidth = sizeof(index) *(mMaxIndexCount * mMaxPreRenderFrameCount);
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		ZeroMemory(&iinitData, sizeof(iinitData));
		iinitData.pSysMem = indexList;
		XGF_Error_Check(mGDI->GetDevice()->CreateBuffer(&ibd, &iinitData, &mIndexBuffer), "CreateIndexBuffer Error");
		//mIndexBuffer = CreateBuffer(mGDI, D3D11_BIND_INDEX_BUFFER, 0, sizeof(index) *(mMaxIndexCount * mMaxPreRenderFrameCount), 0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		PutDebugString(mIndexBuffer);
		delete[] indexList;
	}

}
