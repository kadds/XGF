#pragma once
#include "Defines.hpp"
#include "Shape.hpp"
#include "Shader.hpp"
#include "Batch.hpp"
#include "Polygon.hpp"
#include <vector>
class MeshData :public Shape::Shape
{
public:
	MeshData(unsigned maxUVlen);
	~MeshData();
	void SetMeshPosition(Position center, unsigned width, unsigned height, float baseY);
private:
	unsigned int maxLen;
	virtual void SetPosition(const Point & p) {};
	virtual bool IsInBBox(const Point & p, const XMFLOAT4X4 * matrix) const { return false; };
};

/*
µÿ–ŒÕ¯¬Á‰÷»æ
I have not finished it!
*/
class GridRenderer
{
public:
	GridRenderer();
	~GridRenderer();
	void Initialize(GDI * gdi, float widthPreRect, float heightPreRect, unsigned xcount, unsigned zcount);
	void Shutdown();
	//every frame call it
	void Begin(const WVPMatrix & matrix);
	//every frame call it after call Begin
	void End();

	//call it after call Begin ,before call End
	void DrawGrid(Point & center);

	//void SetYAxisHeight(,float height);
	//void Set
private:
	Batch mBatch;
	Shader mShader;
	//MeshData * mMeshData;
	float mWidth;
	float mHeight;
	PolygonPle *mPolygon;
	PolygonPleIndex *mPolygonPleIndex;
	BindingBridge mBindingBridge;
	PolygonPleConstantExColorBinder *mColorBinder;
	unsigned mXcount;
	unsigned mZcount;
};

