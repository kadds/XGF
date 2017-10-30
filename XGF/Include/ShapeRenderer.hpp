#pragma once
#include"Shader.hpp"
#include "Batch.hpp"
class GDI;
/*
��Color��Ⱦ����ͼ��
Begin��Draw��End
*/
class ShapeRenderer
{
public:
	ShapeRenderer();
	~ShapeRenderer();
	void Initialize(GDI * gdi, unsigned int MaxVetices = 8192, unsigned int MaxIndices = 8192 /4 * 6);
	void Shutdown();

	void Begin(WVPMatrix & wvp);
	void End();
	void Flush();
	void DrawRectangle(float x, float y, float w, float h, Color & color);
	void DrawRectangleB(float x, float y, float w, float h, Color & bkcolor, float borderWidth, Color & boderColor, Color & boderOuterColor);
	void DrawCircle(float x, float y, float r, int precision,Color & color);
	void DrawCircle(float x, float y, float r, int precision, Color & color, Color & centerColor);
	void DrawLine(float x, float y, float endx, float endy, Color & color);
private:
	Batch mBatch;
};

