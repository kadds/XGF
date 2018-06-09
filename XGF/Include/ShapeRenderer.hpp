#pragma once
#include"Shader.hpp"
#include "Batch.hpp"
namespace XGF
{
	class GDI;
	/*
	用Color渲染几何图形2d
	Begin，Draw，End
	*/
	class ShapeRenderer
	{
	public:
		ShapeRenderer();
		~ShapeRenderer();
		void Initialize(GDI * gdi, unsigned int MaxVetices = 8192, unsigned int MaxIndices = 8192 / 4 * 6);
		void Shutdown();

		void Begin(WVPMatrix & wvp);
		void End();
		void Flush();
		void DrawRectangle(float x, float y, float w, float h, float z, const SM::Color & color);
		void DrawRectangleC(float x, float y, float w, float h, float z, const SM::Color & bkcolor, float borderWidth, const SM::Color & boderColor, const SM::Color & boderOuterColor);
		void DrawCircle(float x, float y, float r, int precision, float z, const SM::Color & color);
		void DrawCircle(float x, float y, float r, int precision, float z, const SM::Color & color, const SM::Color & centerColor);
		void DrawLine(float x, float y, float endx, float endy, float z, const SM::Color & color);
		void DrawLineList(const Position &points, int count, float z, const SM::Color & color);
		void DrawTriangle(const Position & a, const Position & b, const Position & c, float z, const SM::Color & ca, const SM::Color & cb, const SM::Color & cc);
		void DrawTriangle(const Position & a, const Position & b, const Position & c, float z, const SM::Color & cc);
		void DrawPolygonList(const Position &points, int count, float z, const SM::Color & color);
	private:
		Batch mBatch;
	};


};
