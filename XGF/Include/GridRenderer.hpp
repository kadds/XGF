#pragma once
#include "Defines.hpp"
#include "Shape.hpp"
#include "Shader.hpp"
#include "Polygon.hpp"
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
		virtual bool IsInBBox(const Point & p, const SM::Vector4 * matrix) const { return false; };
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
		void Initialize(float widthPreRect, float heightPreRect, unsigned xcount, unsigned zcount, const Point & origin = Point(0.f, 0.f, 0.f));
		void Shutdown();
		//every frame call it
		void Begin(const WVPMatrix & matrix);
		//every frame call it after call Begin
		void End();

		//call it after call Begin ,before call End
		void DrawGrid(Point & center);
		void SetColor(SM::Color & cx, SM::Color & cz);
		void SetXColor(SM::Color & cx);
		void SetZColor(SM::Color & cz);
	private:
		ShaderStage mShaderStage;
		//MeshData * mMeshData;
		float mWidth;
		float mHeight;
		std::shared_ptr<PolygonPlePointBinder> mPolygon;
		std::shared_ptr<PolygonPleIndex> mPolygonPleIndex;
		BindingBridge mBindingBridge;
		std::shared_ptr<PolygonPleConstantColorBinder> mColorBinder;
		unsigned mXcount;
		unsigned mZcount;
	};


}
