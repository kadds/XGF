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
		auto vs = mShaders->GetVSShader();
		SlotCan * can = vs->GetSlotCan();
		UINT nsizepos;
		//copy 顶点附加数据
		int itemp = 0;
		for (int i = 0; i < bbridge.GetBinderCount(); i++)
		{
			nsizepos = 0;
			if (can[i].nextIsASlot)
			{
				nsizepos += can[i].size;
				itemp++;
			}
			else
			{
				nsizepos += can[i].size;
				int temp = 0;
				for (int j = 0; j < itemp + 1; j++) 
				{
					bbridge.GetBinder(i - itemp + j)->CopyTo(mVertexData[i] + nsizepos * (mPosInVertices + mLastFrameVBStart) + temp, nsizepos);
					temp += can[i - itemp].size;
				}
				nsizepos = 0;
				itemp = 0;
			}
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

	void Batch::Initialize(GDI * gdi, Shaders * shaders, int MaxVertices, int MaxIndexCount, TopologyMode tm, SOmode sm, InstanceMode im)
	{
		mGDI = gdi;
		mMaxVertices = MaxVertices;
		mMaxIndexCount = MaxIndexCount;
		mShaders = shaders;
		mTopologyMode = tm;
		//初始化
		mLastFrameVBStart = 0;
		mLastFrameIBStart = 0;
		mFramePosition = 0;
		mIsBegin = false;
		mIsMap = false;
		mTextureResource = nullptr;
		mDisabledBlend = false;
		mNullTexture = true;
		mUsingBlend = false;
		mVSShaderResourceView = nullptr;
		mSOMode = sm;
		mInstanceMode = im;

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
			mVertexBufferCount = 1;
			mVertexData = nullptr;
		}
		if (im == InstanceMode::Open)
		{

		}
		CreateIndexBuffer();
		auto vs = shaders->GetVSShader();
		mVertexBufferCount = vs->GetSlotCount();
		mVertexBuffer = new ID3D11Buffer *[mVertexBufferCount];
		UINT prt[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		vs->GetStride(prt);
		for (int i = 0; i < mVertexBufferCount; i++)
		{
			CreateVertexBuffer(prt[i], &mVertexBuffer[i]);
		}
		mVertexData = new VertexDate[mVertexBufferCount];
		mConstantBuffer.Initialize(gdi);

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
	void Batch::EndWithVertexBuffer(ID3D11Buffer * c, int count)
	{

		if (mSOMode == SOmode::SOIn)
		{
			PrepareForRender();
			UINT stride[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
			mShaders->GetVSShader()->GetStride(stride);
			unsigned int offset[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{ 0 };
			mGDI->GetDeviceContext()->IASetVertexBuffers(0, 1, &c, stride, offset);
			mGDI->GetDeviceContext()->DrawAuto();
		}
	}
	void Batch::EndWithVSID3D11ShaderResourceView(ID3D11ShaderResourceView * c, int count)
	{
		mVSShaderResourceView = c;
		mPosInVertices = count;
		End();
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
		if (mUsingIndex)
			mIndexBuffer->Release();
		if (mVertexData != nullptr)
			delete[] mVertexData;
		if (mSOMode == SOmode::SOOut)
		{
			mSOBuffer[0]->Release();
			mSOBuffer[1]->Release();
		}
		else
		{
			for (int i = 0; i < mVertexBufferCount; i++)
			{
				mVertexBuffer[i]->Release();
			}
			delete[] mVertexBuffer;
		}
		mConstantBuffer.Shutdown();
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
		mShaders->GetVSShader()->GetStride(stride);
		unsigned int offset[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]{ 0 };
		unsigned int ct = mVertexBufferCount;
		ID3D11Buffer *mbuffer[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		memset(mbuffer, 0, sizeof(mbuffer));
		mShaders->BindShader();
		mShaders->GetVSShader()->SetInputLayout();
		mConstantBuffer.Map(gdi);
		*mConstantBuffer.GetBufferPoint() = *mMatrix;
		mConstantBuffer.Unmap(gdi);
		mShaders->SetVSConstantBuffer(&mConstantBuffer);
		
		if (ct > 0)
			gdi->GetDeviceContext()->IASetVertexBuffers(ct, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - ct, mbuffer, offset, offset);
		if (mSOMode == SOmode::SOOut)
		{
			gdi->GetDeviceContext()->SOSetTargets(1, mSOBuffer, offset);
			gdi->GetDeviceContext()->IASetVertexBuffers(0, ct, mVertexBuffer, stride, offset);
		}
		else if (mSOMode == SOmode::None)
		{
			ID3D11Buffer *b = 0;
			gdi->GetDeviceContext()->SOSetTargets(1, &b, offset);
			gdi->GetDeviceContext()->IASetVertexBuffers(0, ct, mVertexBuffer, stride, offset);
		}
		else if (mSOMode == SOmode::SOIn)
		{
			ID3D11Buffer *b = 0;
			gdi->GetDeviceContext()->SOSetTargets(1, &b, offset);
			//gdi->GetDeviceContext()->IASetVertexBuffers(0, ct, mVertexBuffer, stride, offset);
		}
		if (mVSShaderResourceView != nullptr)
			gdi->GetDeviceContext()->VSSetShaderResources(0, 1, &mVSShaderResourceView);//TODO::clean
		if (mUsingIndex)
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
		char *vertices = new char[mMaxVertices * len * mMaxPreRenderFrameCount];
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = len * mMaxVertices * mMaxPreRenderFrameCount;
		vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = vertices;
		if (mSOMode == SOmode::SOOut)
			vertexDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		XGF_Error_Check(mGDI->GetDevice()->CreateBuffer(&vertexDesc, &resourceData, buffer), "CreateVertexBuffer Error");
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
		if (mSOMode == SOmode::None)
			if (mUsingIndex)
				mGDI->GetDeviceContext()->DrawIndexed(mPosInIndices - mPosBeforeIndices, mLastFrameIBStart + mPosBeforeIndices, 0);
			else
				mGDI->GetDeviceContext()->Draw(mPosInVertices - mBeforeVertices, mLastFrameVBStart + mBeforeVertices);
		else if (mSOMode == SOmode::SOOut)
		{
			mGDI->GetDeviceContext()->Draw(mPosInVertices - mBeforeVertices, mLastFrameVBStart + mBeforeVertices);
			std::swap(mSOBuffer[0], mSOBuffer[1]);
		}
		ID3D11ShaderResourceView * sr = 0;
		if (mVSShaderResourceView != nullptr)
			mGDI->GetDeviceContext()->VSSetShaderResources(0, 1, &sr);//TODO::clean

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
		PutDebugString(mIndexBuffer);
		delete[] indexList;
	}

}
