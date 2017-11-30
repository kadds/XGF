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

void Batch::DrawPolygon(const BindingBridge & bbridge)
{
	//Map Buffer
	if (!mIsMap) {
		Map(false);
		mIsMap = true;
	}
	else if (mMaxVertices - mPosInVertices <  bbridge.GetBinder(0)->mCount)
	{
		ReportWarning("Out of Range In Vertex. You must set a larger buffer size.");
		return;//TODO::ERROR
	}
	int temp = mShader->GetCountInSlot(0);
	UINT *sizepos;
	UINT nsizepos;
	//copy 顶点附加数据
	int itemp = 0;
	for (int i = 0; i < bbridge.GetBinderCount(); i++)
	{
		temp = mShader->GetCountInSlot(i);
		sizepos = mShader->GetSizeInSlot(i);
		nsizepos = 0;
		UINT slotSize = mShader->GetAllSizeInOneSlot(i);
		for (int j = 0; j < temp; j++)
		{
			bbridge.GetBinder(itemp)->CopyTo(mVertexData[i] + slotSize * (mPosInVertices + mLastFrameVBStart) + nsizepos, slotSize);
			nsizepos += sizepos[j];
			itemp++;
		}
		
	}
	mPosInVertices += bbridge.GetBinder(0)->mCount;
	DebugInscriber_CallAPolygon();
}

void Batch::DrawPolygon(const PolygonPleIndex & pindex, const BindingBridge & bbridge)
{
	if (mMaxIndexCount - mPosInIndices < pindex.mCount)
	{
		ReportWarning("Out of Range In Index. You must set a larger buffer size.");
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

void Batch::Initialize(GDI * gdi, Shader * shader, int MaxVertices, int MaxIndexCount,  TopologyMode tm, SOmode sm)
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
	mSOMode = sm;

	if (sm == SOmode::SOIn)
	{
		mVertexBufferCount = 0;
		mVertexBuffer = nullptr;
		mVertexData = nullptr;
		mUsingZBuffer = true;
		mUsingIndex = false;
		return;
	}
	if (MaxIndexCount <= 0)
	{
		mUsingIndex = false;
	}
	else
		mUsingIndex = true;
	mUsingZBuffer = true;
	if (sm == SOmode::SOOut)
	{
		D3D11_BUFFER_DESC bufferDesc =
		{
			10000,//TODO::max_vec_count
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER,
			0,
			0,
			0
		};
		gdi->GetDevice()->CreateBuffer(&bufferDesc, NULL, &mSOBuffer[0]);
		gdi->GetDevice()->CreateBuffer(&bufferDesc, NULL, &mSOBuffer[1]);
		mUsingIndex = false;
		mUsingZBuffer = false;
	}
	CreateIndexBuffer();
	mVertexBufferCount = shader->GetSlotCount();
	mVertexBuffer = new ID3D11Buffer *[mVertexBufferCount];
	SlotCan *p = shader->GetSizePreSlot();
	UINT prt[16];
	mShader->GetAllSizeInSlot(prt);
	for (int i = 0; i < mVertexBufferCount; i++)
	{
		CreateVertexBuffer(prt[i], &mVertexBuffer[i]);
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
ID3D11Buffer * Batch::GetSOOutBuffer()
{
	return mSOBuffer[0];
}
void Batch::EndWithVertexBuffer(ID3D11Buffer * c)
{
	
	if (mSOMode == SOmode::SOIn)
	{
		PrepareForRender();
		UINT stride[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		mShader->GetAllSizeInSlot(stride);
		unsigned int offset[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{ 0 };
		mGDI->GetDeviceContext()->IASetVertexBuffers(0, 1, &c, stride, offset);
		mGDI->GetDeviceContext()->DrawAuto();
	}
		
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
	if(mVertexData != nullptr)
		delete[] mVertexData;

	for (int i = 0; i < mVertexBufferCount; i++)
	{
		mVertexBuffer[i]->Release();
	}
	if (mSOMode == SOmode::SOOut)
	{
		mSOBuffer[0]->Release();
		mSOBuffer[1]->Release();
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
	UINT stride[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	mShader->GetAllSizeInSlot(stride);
    unsigned int offset[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{0};
    unsigned int ct = mVertexBufferCount;
	ID3D11Buffer *mbuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
	memset(mbuffer, 0, sizeof(mbuffer));
    mShader->SetShaderParameter(*mMatrix);
    mShader->SetShaderAndInputLayout();
	if(ct > 0)
		gdi->GetDeviceContext()->IASetVertexBuffers(ct, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - ct, mbuffer, offset, offset);
	if (mSOMode == SOmode::SOOut)
	{
		gdi->GetDeviceContext()->SOSetTargets(1, mSOBuffer, offset);
		gdi->GetDeviceContext()->IASetVertexBuffers(0, ct, mVertexBuffer, stride, offset);
	}
	else if(mSOMode == SOmode::None)
	{
		ID3D11Buffer *b = 0;
		gdi->GetDeviceContext()->SOSetTargets(1, &b, offset);
		gdi->GetDeviceContext()->IASetVertexBuffers(0, ct, mVertexBuffer, stride, offset);
	}
	else if(mSOMode == SOmode::SOIn)
	{
		ID3D11Buffer *b = 0;
		gdi->GetDeviceContext()->SOSetTargets(1, &b, offset);
		//gdi->GetDeviceContext()->IASetVertexBuffers(0, ct, mVertexBuffer, stride, offset);
	}
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
	if(mSOMode == SOmode::None)
		if (mUsingIndex)
			mGDI->GetDeviceContext()->DrawIndexed(mPosInIndices - mPosBeforeIndices, mLastFrameIBStart + mPosBeforeIndices, 0);
		else
		mGDI->GetDeviceContext()->Draw(mPosInVertices - mBeforeVertices, mLastFrameVBStart + mBeforeVertices);
	else if (mSOMode == SOmode::SOOut)
	{
		mGDI->GetDeviceContext()->Draw(mPosInVertices - mBeforeVertices, mLastFrameVBStart + mBeforeVertices);
		std::swap(mSOBuffer[0], mSOBuffer[1]);
	}
		
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
