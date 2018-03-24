#pragma once
#include "Defines.hpp"
#include "Shape.hpp"
#include "Shader.hpp"
#include "Batch.hpp"
#include "Polygon.hpp"
#include <vector>
namespace XGF
{
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
		void Initialize(GDI * gdi, float widthPreRect, float heightPreRect, unsigned xcount, unsigned zcount, const Point & origin = Point(0.f, 0.f, 0.f));
		void Shutdown();
		//every frame call it
		void Begin(const WVPMatrix & matrix);
		//every frame call it after call Begin
		void End();

		//call it after call Begin ,before call End
		void DrawGrid(Point & center);
		void SetColor(Color & cx, Color & cz);
		void SetXColor(Color & cx);
		void SetZColor(Color & cz);
	private:
		Batch mBatch;
		//MeshData * mMeshData;
		float mWidth;
		float mHeight;
		PolygonPlePoint3 *mPolygon;
		PolygonPleIndex *mPolygonPleIndex;
		BindingBridge mBindingBridge;
		PolygonPleConstantExColorBinder *mColorBinder;
		unsigned mXcount;
		unsigned mZcount;
	};


}
