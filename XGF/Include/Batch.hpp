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
enum RenderMode {Buffer = 0, RealTime};

typedef D3D_PRIMITIVE_TOPOLOGY TopologyMode;

//用于顶点缓存信息节点
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
enum MapMode 
{
	Discard,
	Overwrite
};
/*
渲染批次类
两种渲染模式：实时模式和缓冲模式
实时模式使用DrawPolygon渲染图形
缓冲模式使用GetFreePosition获取一个可用的顶点缓存， 可以调用WriteToVertexBuffer 更新顶点缓存，每帧必须调用 WriteToIndexBuffer 更新索引
缓冲模式有BUG，不建议使用
所有导致状态改变的函数可能间接调用Flush函数，从而增加Call Batch的机会
请搭配Begin End 使用
每个Bach不能动态改变TopologyMode类型和RenderMode类型
*/
class Batch
{
public:
	Batch();
	//使状态改变
    void SetTexture(const Texture & tex);
	//使状态改变
    void SetTexture(ID3D11ShaderResourceView* rv);

    int GetFreePosition(int len);
    int WriteToVertexBuffer(int slot, int pos, void * buffer, int len, VertexTransfrom transfrom);
    int WriteToIndexBuffer(index * buffer, int len, int off);
	void DrawPolygon(const PolygonPle & polygon, const PolygonPleIndex & pindex, const BindingBridge & bbridge);
	//提前提交图形
	void Flush();
	Shader * GetShader() const{ return mShader; }
	void Initialize(GDI * gdi, Shader * shader, int MaxVertices, int MaxIndexCount, RenderMode em = RenderMode::RealTime, bool UsingZBuffer = true, TopologyMode tm = TopologyMode::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void Shutdown();
	
	void Begin(const WVPMatrix & Matrix);
	void End();
	//使状态改变
	void SetBlend(bool isOpen);
	
	void SetMapMode(MapMode md) { mMapMode = md; };
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
protected:
	//保存上一次渲染的纹理，便于合并Batch
    ID3D11ShaderResourceView* mTextureResource;
	GDI * mGDI;

	ID3D11Buffer ** mVertexBuffer;
	ID3D11Buffer * mIndexBuffer;
	//外部Shader
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

	MapMode mMapMode;
	RenderMode mUsingMode;
	TopologyMode mTopologyMode;

    index *mIndexData;
protected:
    void CreateIndexBuffer();
    void CreateVertexBuffer(unsigned len, ID3D11Buffer ** buffer);
private:
	WVPMatrix* mMatrix;
	void Map(bool discard);
	void UnMap();
    void PrepareForRender();
	DISALLOW_COPY_AND_ASSIGN(Batch);
	//保存未使用的顶点缓存信息
    LinkedList<BufferInformation> mBufferState;
};
