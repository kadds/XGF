#pragma once
#include "Shader.hpp"
namespace XGF
{
	/*
	用Color渲染几何图形2d
	Begin，Draw，End
	*/
	class ShapeRenderer
	{
	public:
		ShapeRenderer();
		~ShapeRenderer();
		void Initialize(unsigned int MaxVetices = 8192, unsigned int MaxIndices = 8192 / 4 * 6);
		void Shutdown();

		void Begin(WVPMatrix & wvp);
		void End();
		void Flush();
		void DrawRectangle(float x, float y, float w, float h, float z, const Color & color);
		void DrawRectangleC(float x, float y, float w, float h, float z, const Color & bkcolor, float borderWidth, const Color & boderColor, const Color & boderOuterColor);
		void DrawCircle(float x, float y, float r, int precision, float z, const Color & color);
		void DrawCircle(float x, float y, float r, int precision, float z, const Color & color, const Color & centerColor);
		void DrawLine(float x, float y, float endx, float endy, float z, const Color & color);
		void DrawLineList(const Position &points, int count, float z, const Color & color);
		void DrawTriangle(const Position & a, const Position & b, const Position & c, float z, const Color & ca, const Color & cb, const Color & cc);
		void DrawTriangle(const Position & a, const Position & b, const Position & c, float z, const Color & cc);
		void DrawPolygonList(const Position &points, int count, float z, const Color & color);
	private:
		ShaderStage mShaderStage;
	};


};
