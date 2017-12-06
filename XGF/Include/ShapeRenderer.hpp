#pragma once
#include"Shader.hpp"
#include "Batch.hpp"
namespace XGF
{
	class GDI;
	/*
	用Color渲染几何图形
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
		void DrawRectangle(float x, float y, float w, float h, float z, Color & color);
		void DrawRectangleB(float x, float y, float w, float h, float z, Color & bkcolor, float borderWidth, Color & boderColor, Color & boderOuterColor);
		void DrawCircle(float x, float y, float r, int precision, float z, Color & color);
		void DrawCircle(float x, float y, float r, int precision, float z, Color & color, Color & centerColor);
		void DrawLine(float x, float y, float endx, float endy, float z, Color & color);
	private:
		Batch mBatch;
	};


};
