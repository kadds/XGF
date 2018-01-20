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
	//��ʱ������
	enum class InstanceMode
	{
		None,
		Open
	};
	/*
	��Ⱦ������
	ʹ��DrawPolygonд�뻺�棬��Ⱦͼ��
	���е���״̬�ı�ĺ������ܼ�ӵ���Flush�������Ӷ�����Call Batch�Ĵ���
	�����Begin End ʹ��
	δ��� Instance Render
	*/
	class Batch
	{
	public:
		Batch();

		void DrawPolygon(const BindingBridge & bbridge);
		void DrawPolygon(const PolygonPleIndex & pindex, const BindingBridge & bbridge);
		//��ǰ�ύͼ��
		void Flush();
		ShaderStage * GetShaderStage() { return &mShaderStage; }
		void Initialize(GDI * gdi, Shaders shaders, int MaxVertices, int MaxIndexCount, TopologyMode tm = TopologyMode::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		void Shutdown();

		void Begin();
		void End();
		//ʹ״̬�ı�
		//�ı�����ģʽ���ᵼ��Flush
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
