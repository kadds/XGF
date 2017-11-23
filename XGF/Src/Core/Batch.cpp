#include "../../Include/Batch.hpp"
#include "../../Include/Log.hpp"
#include <DirectXMath.h>
#include "../../Include/DebugInscriber.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Texture.hpp"
#include "../../Include/Shader.hpp"
#include "../../Include/Polygon.hpp"
#include <algorithm>
int Batch::mClientWidth;
int Batch::mClientHeight;
unsigned int Batch::mMaxPreRenderFrameCount = 3;

void Batch::DrawPolygon(const PolygonPle & polygon, const BindingBridge & bbridge)
{
	//Map Buffer
	if (!mIsMap) {
		Map(false);
		mIsMap = true;
	}
	else if (mMaxVertices - mPosInVertices <  polygon.mCount)
	{
		ReportWarning("Out of Range In Vertex. You must set a larger buffer size.");
		return;//TODO::ERROR
	}
	//copy 顶点位置数据
	unsigned slotSize = mShader->GetSizeInSlot(0);
	memcpy(mVertexData[0] + slotSize * (mPosInVertices + mLastFrameVBStart), polygon.mPoint, slotSize * polygon.mCount);
	//copy 顶点附加数据
	for (int i = 0; i < bbridge.GetBinderCount(); i++)
	{
		slotSize = mShader->GetSizeInSlot(i + 1);
		bbridge.GetBinder(i)->CopyTo(mVertexData[i + 1] + slotSize * (mPosInVertices + mLastFrameVBStart));
	}
	mPosInVertices += polygon.mCount;
	DebugInscriber_CallAPolygon();
}

void Batch::DrawPolygon(const PolygonPle & polygon, const PolygonPleIndex & pindex, const BindingBridge & bbridge)
{
	if (mMaxIndexCount - mPosInIndices < pindex.mCount)
	{
		ReportWarning("Out of Range In Index. You must set a larger buffer size.");
		return;//TODO::ERROR
	}
	DrawPolygon(polygon, bbridge);
	pindex.CopyTo(mIndexData + mPosInIndices + mLastFrameIBStart, mPosInVertices - polygon.mCount + mLastFrameVBStart);
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

void Batch::Initialize(GDI * gdi, Shader * shader, int MaxVertices, int MaxIndexCount,  TopologyMode tm)
{
	mGDI = gdi;
	mMaxVertices = MaxVertices;
	mMaxIndexCount = MaxIndexCount;
	mShader = shader;
	mTopologyMode = tm;
	
	//初始化
	mLastFrameVBStart = 0;
	mLastFrameIBStart = 0;
	mFramePosition  = 0;
	mIsBegin = false;
	mIsMap = false;
	mTextureResource = nullptr;
	mDisabledBlend = false;
	mNullTexture = true;
	mUsingBlend = false;
	mUsingZBuffer  = true;
	if (MaxIndexCount <= 0)
	{
		mUsingIndex = false;
	}
	else
		mUsingIndex = true;
	CreateIndexBuffer();
    mVertexBufferCount = shader->GetSlotCount();
    mVertexBuffer = new ID3D11Buffer *[mVertexBufferCount];
    const unsigned *p = shader->GetSizePreSlot();
    for (int i = 0; i < mVertexBufferCount; i++)
    {
        CreateVertexBuffer(p[i],&mVertexBuffer[i]);
    }
    mVertexData = new VertexDate[mVertexBufferCount];
}
void Batch::Begin(const WVPMatrix & Matrix)
{
	if (mIsBegin)
		return;//TODO::ERROR
	mIsBegin = true;
	mMatrix = const_cast<WVPMatrix *>(&Matrix);
    mPosInIndices = 0;
	mPosInVertices = 0;
    mPosBeforeIndices = 0;
	mBeforeVertices = 0;
}
void Batch::End()
{
	EndWithoutFrame();
    DebugInscriber_CallABatch(mPosInIndices, mPosInVertices);
	
	mLastFrameVBStart += mPosInVertices ;
	mLastFrameIBStart += mPosInIndices ;
	mFramePosition++;
	if (mFramePosition >= mMaxPreRenderFrameCount)
	{
		mFramePosition = 0;
		mLastFrameVBStart = 0;
		mLastFrameIBStart = 0;
	}

}
void Batch::SetBlend(bool Open)
{
	if (mUsingBlend != Open)
	{
		Flush();
	}
	mUsingBlend = Open;
}
void Batch::ChangeTopologyMode(TopologyMode tm)
{
	if (mTopologyMode != tm)
	{
		Flush();
		mTopologyMode = tm;
	}
}
void Batch::SetZBufferRender(bool open)
{
	mUsingZBuffer = open;
}
TopologyMode Batch::GetTopologyMode()
{
	return mTopologyMode;
}
Batch::Batch()
{
}
void Batch::SetTexture(const Texture & tex)
{
	SetTexture(tex.GetShaderResourceView());
}
void Batch::SetTexture(ID3D11ShaderResourceView * rv)
{
	if (!mIsBegin) return;
	if (rv == nullptr)
	{
		if (mTextureResource != nullptr)
		{
			Flush();
		}
		mNullTexture = true;
		return;
	}
	if (mTextureResource == nullptr || mTextureResource != rv)
    {
		Flush();
		mTextureResource = rv;
		mNullTexture = false;
    }
}
void Batch::Shutdown()
{
	if(mUsingIndex)
		mIndexBuffer->Release();
    delete[] mVertexData;
    for (int i = 0; i < mVertexBufferCount; i++)
    {
        mVertexBuffer[i]->Release();
    }
    delete[] mVertexBuffer;
}

void Batch::Map(bool discard)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
    for (int i = 0; i < mVertexBufferCount; i++)
    {
        mGDI->GetDeviceContext()->Map(mVertexBuffer[i], 0, discard ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
        mVertexData[i] = static_cast<char*>(mappedResource.pData);
    }
	if (mUsingIndex)
	{
		mGDI->GetDeviceContext()->Map(mIndexBuffer, 0, discard ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
		mIndexData = static_cast<index *>(mappedResource.pData);
	}
}

void Batch::UnMap()
{
    for (int i = 0; i < mVertexBufferCount; i++)
    {
        mGDI->GetDeviceContext()->Unmap(mVertexBuffer[i], 0);
    }
	if (mUsingIndex)
	{
		mGDI->GetDeviceContext()->Unmap(mIndexBuffer, 0);
	}
	
}
void Batch::PrepareForRender()
{
    auto gdi = mGDI;
    unsigned int* stride = mShader->GetSizePreSlot();
    unsigned int offset[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned int ct = mVertexBufferCount;
	unsigned int stride2[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	ID3D11Buffer *mbuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	memset(mbuffer, 0, sizeof(mbuffer));
	memset(stride2, 0, sizeof(stride2));
    mShader->SetShaderParameter(*mMatrix);
    mShader->SetShaderAndInputLayout();
    gdi->GetDeviceContext()->IASetVertexBuffers(0, ct, mVertexBuffer, stride, offset);
	//清楚以前VB的缓存
	gdi->GetDeviceContext()->IASetVertexBuffers(ct , D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - ct, mbuffer, stride2, offset);
	if(mUsingIndex)
		gdi->GetDeviceContext()->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    gdi->GetDeviceContext()->IASetPrimitiveTopology(mTopologyMode);
	if (mTextureResource)
	{
		gdi->SetDefaultSamplerState();
		mGDI->GetDeviceContext()->PSSetShaderResources(0, 1, &mTextureResource);
	}
	if (mNullTexture) {
		ID3D11ShaderResourceView *const pSRV[1] = { NULL };
		gdi->GetDeviceContext()->PSSetShaderResources(0, 1, pSRV);
	} 
	if (mUsingBlend)
		gdi->SetBlendState(BlendState::AddZeroOneAdd);
	else
		gdi->SetBlendState(BlendState::NoneBlend);
}

void Batch::CreateVertexBuffer(unsigned len, ID3D11Buffer ** buffer)
{
    D3D11_BUFFER_DESC vertexDesc;
	mMaxVertices *= mMaxPreRenderFrameCount;
    char *vertices = new char[mMaxVertices * len];
    ZeroMemory(&vertexDesc, sizeof(vertexDesc));
    vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.ByteWidth = len * mMaxVertices;
    vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_SUBRESOURCE_DATA resourceData;
    ZeroMemory(&resourceData, sizeof(resourceData));
    resourceData.pSysMem = vertices;
    Check(mGDI->GetDevice()->CreateBuffer(&vertexDesc, &resourceData, buffer));
    PutDebugString((*buffer));
    delete[] vertices;
}

void Batch::EndWithoutFrame()
{
	if (!mIsBegin)
		return;//TODO::ERROR
	mIsBegin = false;

	if (mIsMap) {
		UnMap();
		mIsMap = false;
	}
	if (mPosInIndices == 0 && mUsingIndex)
		return;
	PrepareForRender();
	mGDI->SetZBufferMode(mUsingZBuffer);
	if (mUsingIndex)
		mGDI->GetDeviceContext()->DrawIndexed(mPosInIndices - mPosBeforeIndices, mLastFrameIBStart + mPosBeforeIndices, 0);
	else
		mGDI->GetDeviceContext()->Draw(mPosInVertices - mBeforeVertices, mLastFrameVBStart + mBeforeVertices);
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
    // 设定用于初始化索引缓冲的数据
    D3D11_SUBRESOURCE_DATA iinitData;
    ZeroMemory(&iinitData, sizeof(iinitData));
    iinitData.pSysMem = indexList;
    // 创建索引缓冲
    Check(mGDI->GetDevice()->CreateBuffer(&ibd, &iinitData, &mIndexBuffer));
    PutDebugString(mIndexBuffer);
    delete[] indexList;
}
