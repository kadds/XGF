#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>

#include "LinkedList.hpp"
#include <vector>
#include "Polygon.hpp"
using std::vector;

class GDI;
class PolygonPle;
struct WVPMatrix;
class Polygon;
class Shader;
class Texture;
class BindingBridge;
typedef char* VertexDate;

typedef D3D_PRIMITIVE_TOPOLOGY TopologyMode;

//���ڶ��㻺����Ϣ�ڵ�
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

enum VertexTransfrom
{
    None,
    Constant
};
/*
��Ⱦ������
ʹ��DrawPolygon��Ⱦͼ��
���е���״̬�ı�ĺ������ܼ�ӵ���Flush�������Ӷ�����Call Batch�Ļ���
�����Begin End ʹ��
*/
class Batch
{
public:
	Batch();
	//ʹ״̬�ı�
    void SetTexture(const Texture & tex);
	//ʹ״̬�ı�
    void SetTexture(ID3D11ShaderResourceView* rv);

    
	void DrawPolygon(const PolygonPle & polygon, const PolygonPleIndex & pindex, const BindingBridge & bbridge);
	//��ǰ�ύͼ��
	void Flush();
	Shader * GetShader() const{ return mShader; }
	void Initialize(GDI * gdi, Shader * shader, int MaxVertices, int MaxIndexCount, TopologyMode tm = TopologyMode::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void Shutdown();
	
	void Begin(const WVPMatrix & Matrix);
	void End();
	//ʹ״̬�ı�
	void SetBlend(bool isOpen);
	//�ı�����ģʽ���ᵼ��Flush
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

	static void SetClientSize(SIZE size) { mClientHeight = size.cy; mClientWidth = size.cx; }
protected:
    static int mClientWidth;
    static int mClientHeight;
	static unsigned int mMaxPreRenderFrameCount;
protected:
	//������һ����Ⱦ���������ںϲ�Batch
    ID3D11ShaderResourceView* mTextureResource;
	GDI * mGDI;

	ID3D11Buffer ** mVertexBuffer;
	ID3D11Buffer * mIndexBuffer;
	//�ⲿShader
	Shader *mShader;

	int mMaxVertices;
	int mMaxIndexCount;
    //int mNowFrame;
	
    int mVertexBufferCount;
    VertexDate *mVertexData;
	int mPosInVertices;
	int mPosInIndices;
    int mPosBeforeIndices;

	bool mIsBegin;
	bool mIsMap;
	bool mUsingZBuffer;
	bool mDisabledBlend;
	bool mNullTexture;
	bool mUsingBlend;

	TopologyMode mTopologyMode;

    index *mIndexData;
	int mLastFrameVBStart;
	int mLastFrameIBStart;
	int mFramePosition;
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
	//����δʹ�õĶ��㻺����Ϣ
    //LinkedList<BufferInformation> mBufferState;
};
