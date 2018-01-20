#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>

#include "LinkedList.hpp"
#include <vector>
#include "Buffer.hpp"
#include "Polygon.hpp"
#include "Shader.hpp"
namespace XGF
{
	using std::vector;

	class GDI;
	struct WVPMatrix;
	class Polygon;
	class ShaderStage;
	class Texture;
	class BindingBridge;
	typedef char* VertexDate;

	typedef D3D_PRIMITIVE_TOPOLOGY TopologyMode;
	//暂时不可用
	enum class InstanceMode
	{
		None,
		Open
	};
	/*
	渲染批次类
	使用DrawPolygon写入缓存，渲染图形
	所有导致状态改变的函数可能间接调用Flush函数，从而增加Call Batch的次数
	请搭配Begin End 使用
	未完成 Instance Render
	*/
	class Batch
	{
	public:
		Batch();

		void DrawPolygon(const BindingBridge & bbridge);
		void DrawPolygon(const PolygonPleIndex & pindex, const BindingBridge & bbridge);
		//提前提交图形
		void Flush();
		ShaderStage * GetShaderStage() { return &mShaderStage; }
		void Initialize(GDI * gdi, Shaders shaders, int MaxVertices, int MaxIndexCount, TopologyMode tm = TopologyMode::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		void Shutdown();

		void Begin();
		void End();
		//使状态改变
		//改变拓扑模式，会导致Flush
		void ChangeTopologyMode(TopologyMode tm);
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
		void End(ID3D11Buffer * c, int count = 0);
		void StepVetices(int count);
		static void SetClientSize(SIZE size) { mClientHeight = size.cy; mClientWidth = size.cx; }
	protected:
		static int mClientWidth;
		static int mClientHeight;
		static unsigned int mMaxPreRenderFrameCount;
	protected:

		GDI * mGDI;
		ID3D11Buffer *mVertexBuffer;
		ID3D11Buffer * mIndexBuffer;
		//ShaderStage 
		ShaderStage mShaderStage;

		int mMaxVertices;
		int mMaxIndexCount;

		VertexDate mVertexData;
		int mPosInVertices;
		int mPosInIndices;
		int mPosBeforeIndices;
		int mBeforeVertices;

		bool mIsBegin;
		bool mIsMap;
		bool mUsingZBuffer;

		bool mUsingIndex;
		TopologyMode mTopologyMode;

		index *mIndexData;
		int mLastFrameVBStart;
		int mLastFrameIBStart;
		unsigned int mFramePosition;
	protected:
		void CreateIndexBuffer();
		void CreateVertexBuffer(unsigned len, ID3D11Buffer ** buffer);
		void EndWithoutFrame(bool drawAuto = false);
	private:
		void Map(bool discard);
		void UnMap();
		void PrepareForRender();
		DISALLOW_COPY_AND_ASSIGN(Batch);
	};

}
