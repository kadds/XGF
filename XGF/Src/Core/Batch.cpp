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

int Batch::GetFreePosition(int len)
{
	if (mUsingMode != RenderMode::Buffer) {
		ReportWarning("Batch Error Render Mode");
		return -1;
	}
    auto it = mBufferState.Begin();
    int pos = -1;
    while (it != mBufferState.End())
    {
        if (it->data.len >= len)
        {
            pos = it->data.start;
            it->data.len -= len;
            it->data.start += len;
            if (it->data.len <= 0)
                mBufferState.Delete(it);
            else
                mBufferState.Combine(it);
            break;
        }
    }
	if (pos == -1)
		ReportWarning(ERROR_BATCH_MEM_OUT_OF_RANGE);
    return pos;
}
int Batch::WriteToVertexBuffer(int slot, int pos , void * buffer, int len, VertexTransfrom transfrom)
{
	if (mUsingMode != RenderMode::Buffer) {
		ReportWarning("Batch Error Render Mode");
		return -1;
	}
    if (!mIsMap) {
		if (mMapMode == Discard)
			Map(true);
		else
			Map(false);
        mIsMap = true;
    }
    float*  bf = (float*) buffer;
    VertexDate vd = mVertexData[slot];
    unsigned slotSize = mShader->GetSizeInSlot(slot);
    float * posiv = ((float *) ((vd + (pos) * slotSize)));
    switch (transfrom)
    {
    case VertexTransfrom::None:
        memcpy(posiv, buffer, len * slotSize);
        break;
    case VertexTransfrom::Constant:
        for (int i = 0; i < len; i++)
        {
            bf = (float *)buffer;
            for (unsigned j = 0; j < slotSize / sizeof(float); j++)
            {
                *posiv = *bf;
                posiv ++;
                bf++;
            }
        }
        break;
    default:
        break;
    } 
    
	return pos;
}
int Batch::WriteToIndexBuffer(index * buffer, int len, int off)
{
	if (mUsingMode != RenderMode::Buffer) {
		ReportWarning("Batch Error Render Mode");
		return -1;
	}

    if (!mIsMap) {
		if(mMapMode == Discard)
			Map(true);
		else
			Map(false);
        mIsMap = true;
    }
	
    for (int i = 0; i < len; i++)
    {
        mIndexData[mPosInIndices + i] = buffer[i] + off;
    }
    mPosInIndices += len;
    DebugInscriber_CallAPolygon();
     return mPosInIndices - len;
}

void Batch::DrawPolygon(const PolygonPle & polygon, const PolygonPleIndex & pindex, const BindingBridge & bbridge)
{
	if (mUsingMode != RenderMode::RealTime) {
		ReportWarning("Batch Error Render Mode");
		return ;
	}
	if (!mIsMap) {
		if (mMapMode == Discard)
			Map(true);
		else
			Map(false);
		mIsMap = true;
	}
	else if(mMaxVertices - mPosInVertices <  polygon.mCount || mMaxIndexCount - mPosInIndices < pindex.mCount)
	{
		ReportWarning("Out of Range In Vertex Or Index Buffer, You Must Set A larger Buffer Size.");
		return ;//TODO::ERROR
	}
	unsigned slotSize = mShader->GetSizeInSlot(0);
	memcpy(mVertexData[0] + slotSize * mPosInVertices, polygon.mPoint, slotSize * polygon.mCount);
	
	for (int i = 0; i < bbridge.GetBinderCount(); i++)
	{
		slotSize = mShader->GetSizeInSlot(i + 1);
		bbridge.GetBinder(i)->CopyTo(mVertexData[i + 1] + slotSize * mPosInVertices);
	}
	pindex.CopyTo(mIndexData + mPosInIndices, mPosInVertices);
	mPosInIndices += pindex.mCount;
	mPosInVertices += polygon.mCount;
	DebugInscriber_CallAPolygon();
}

void Batch::Flush()
{
	if (mIsBegin)
	{
		End();
		mPosBeforeIndices = mPosInIndices;
		mIsBegin = true;
	}
}

void Batch::Initialize(GDI * gdi, Shader * shader, int MaxVertices, int MaxIndexCount, RenderMode em, bool UsingZBuffer, TopologyMode tm)
{
	mGDI = gdi;
	mMaxVertices = MaxVertices;
	mMaxIndexCount = MaxIndexCount;
	mShader = shader;
	mUsingMode = em; 
	mUsingZBuffer = UsingZBuffer;
	mTopologyMode = tm;
	CreateIndexBuffer();
	
	
    mVertexBufferCount = shader->GetSlotCount();
    mVertexBuffer = new ID3D11Buffer *[mVertexBufferCount];
    const unsigned *p = shader->GetSizePreSlot();
    for (int i = 0; i < mVertexBufferCount; i++)
    {
        CreateVertexBuffer(p[i],&mVertexBuffer[i]);
    }
    mVertexData = new VertexDate[mVertexBufferCount];
	if (mUsingMode == RenderMode::Buffer)
		mBufferState.Insert(BufferInformation(0, mMaxVertices));
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
}
void Batch::End()
{
    if (!mIsBegin)
        return;//TODO::ERROR
    mIsBegin = false;
    
    if (mIsMap) {
        UnMap();
        mIsMap = false;
    }
	if (mPosInIndices == 0)
		return;
    PrepareForRender();
	mGDI->SetZBufferMode(mUsingZBuffer);
	mGDI->GetDeviceContext()->DrawIndexed(mPosInIndices - mPosBeforeIndices, mPosBeforeIndices, 0);
    DebugInscriber_CallABatch(mPosInIndices - mPosBeforeIndices, mPosInVertices);

}
void Batch::SetBlend(bool Open)
{
	if (mUsingBlend != Open)
	{
		Flush();
	}
	mUsingBlend = Open;
}
Batch::Batch() :mIsBegin(false), mIsMap(false), mTextureResource(nullptr),mMapMode(Discard), mDisabledBlend(false), mNullTexture(true),mUsingBlend(false)
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
	
	mGDI->GetDeviceContext()->Map(mIndexBuffer, 0, discard ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
    mIndexData = static_cast<index *>(mappedResource.pData);
}

void Batch::UnMap()
{
    for (int i = 0; i < mVertexBufferCount; i++)
    {
        mGDI->GetDeviceContext()->Unmap(mVertexBuffer[i], 0);
    }
	mGDI->GetDeviceContext()->Unmap(mIndexBuffer, 0);
}
void Batch::PrepareForRender()
{
    auto gdi = mGDI;
    unsigned int* stride = mShader->GetSizePreSlot();
    unsigned int offset[16]{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned int ct = mVertexBufferCount;
    mShader->SetShaderParameter(*mMatrix);
    mShader->SetShaderAndInputLayout();
    gdi->GetDeviceContext()->IASetVertexBuffers(0, ct, mVertexBuffer, stride, offset);
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
		gdi->OpenDefaultBlendState();
	else
		gdi->CloseBlendState();
}

void Batch::CreateVertexBuffer(unsigned len, ID3D11Buffer ** buffer)
{
    D3D11_BUFFER_DESC vertexDesc;
    char *vertices = new char[mMaxVertices * len];
    //ZeroMemory(mVertices, sizeof(VertexPosPCT)*mMaxVertices);
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

void Batch::CreateIndexBuffer()
{
    /*UINT index[]{
    1, 2, 0,
    0, 3, 1
    };*/
    index *indexList = new index[mMaxIndexCount];
    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_DYNAMIC;
    ibd.ByteWidth = sizeof(index) *(mMaxIndexCount);
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
