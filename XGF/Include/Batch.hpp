#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>

#include "LinkedList.hpp"
#include <vector>
#include "Buffer.hpp"
#include "Polygon.hpp"
namespace XGF
{
	using std::vector;

	class GDI;
	struct WVPMatrix;
	class Polygon;
	class Shaders;
	class Texture;
	class BindingBridge;
	typedef char* VertexDate;

	typedef D3D_PRIMITIVE_TOPOLOGY TopologyMode;

	//用于顶点缓存信息节点,该结构体废弃
	class BufferInformation {
	public:
		int start;
		int len;
		BufferInformation() :start(1000000), len(0)
		{
		}
		BufferInformation(int s, int l) :start(s), len(l)
		{
		}
		BufferInformation& operator+= (const BufferInformation& bf)
		{
			this->len += bf.len;
			return *this;
		}
		BufferInformation& operator-= (const BufferInformation& bf)
		{
			this->len -= bf.len;
			return *this;
		}
		bool operator< (const BufferInformation& bf) const
		{
			return this->start < bf.start;
		}
		bool CanCombine(const BufferInformation& bf) const
		{
			return start + len >= bf.start;
		}

	};
	enum class SOmode
	{
		None,
		SOOut,
		SOIn
	};
	enum class VertexTransfrom
	{
		None,
		Constant
	};
	enum class InstanceMode
	{
		None,
		Open
	};
	/*
	渲染批次类
	使用DrawPolygon渲染图形
	所有导致状态改变的函数可能间接调用Flush函数，从而增加Call Batch的机会
	请搭配Begin End 使用
	*/
	class Batch
	{
	public:
		Batch();
		//使状态改变
		void SetTexture(const Texture & tex);
		//使状态改变
		void SetTexture(ID3D11ShaderResourceView* rv);

		void DrawPolygon(const BindingBridge & bbridge);
		void DrawPolygon(const PolygonPleIndex & pindex, const BindingBridge & bbridge);
		//提前提交图形
		void Flush();
		Shaders * GetShaders() const { return mShaders; }
		void Initialize(GDI * gdi, Shaders * shaders, int MaxVertices, int MaxIndexCount, TopologyMode tm = TopologyMode::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, SOmode smode = SOmode::None, InstanceMode im = InstanceMode::None);
		void Shutdown();

		void Begin(const WVPMatrix & Matrix);
		void End();
		//使状态改变
		void SetBlend(bool isOpen);
		//改变拓扑模式，会导致Flush
		void ChangeTopologyMode(TopologyMode tm);
		void SetZBufferRender(bool open);
		TopologyMode GetTopologyMode();
		static int GetClientWidth() {
			return mClientWidth;
		}
		static int GetClientHeight() {
			return mClientHeight;
		}

		static float GetClientWidthD2() {
			return mClientWidth / 2.f;
		}
		static float GetClientHeightD2() {
			return mClientHeight / 2.f;
		}
		ID3D11Buffer * GetSOOutBuffer();
		void EndWithVertexBuffer(ID3D11Buffer * c, int count = 0);
		void EndWithVSID3D11ShaderResourceView(ID3D11ShaderResourceView * c, int count);
		static void SetClientSize(SIZE size) { mClientHeight = size.cy; mClientWidth = size.cx; }
	protected:
		static int mClientWidth;
		static int mClientHeight;
		static unsigned int mMaxPreRenderFrameCount;
	protected:
		//保存上一次渲染的纹理，便于合并Batch
		ID3D11ShaderResourceView* mTextureResource;

		ID3D11ShaderResourceView * mVSShaderResourceView;
		GDI * mGDI;
		ID3D11Buffer ** mVertexBuffer;
		ID3D11Buffer *mSOBuffer[2];
		ID3D11Buffer * mIndexBuffer;
		ConstantBuffer<WVPMatrix> mConstantBuffer;
		//外部Shader
		Shaders *mShaders;

		int mMaxVertices;
		int mMaxIndexCount;
		//int mNowFrame;

		int mVertexBufferCount;
		VertexDate *mVertexData;
		int mPosInVertices;
		int mPosInIndices;
		int mPosBeforeIndices;
		int mBeforeVertices;

		bool mIsBegin;
		bool mIsMap;
		bool mUsingZBuffer;
		bool mDisabledBlend;

		bool mNullTexture;
		bool mUsingBlend;
		bool mUsingIndex;
		SOmode mSOMode;
		TopologyMode mTopologyMode;
		InstanceMode mInstanceMode;

		index *mIndexData;
		int mLastFrameVBStart;
		int mLastFrameIBStart;
		unsigned int mFramePosition;
	protected:
		void CreateIndexBuffer();
		void CreateVertexBuffer(unsigned len, ID3D11Buffer ** buffer);
		void EndWithoutFrame();
	private:
		WVPMatrix* mMatrix;
		void Map(bool discard);
		void UnMap();
		void PrepareForRender();
		DISALLOW_COPY_AND_ASSIGN(Batch);
	};

}
