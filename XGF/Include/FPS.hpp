#pragma once
#include "Defines.hpp"
class TextRenderer;
/*
*此类用于计算Fps 
*/
class FPS
{
public:
	FPS();
	~FPS();
	void Tick(float delat);
	void RenderFPS(TextRenderer & TextRenderer);
	void RenderFPS(TextRenderer & TextRenderer,int x , int y);

	void RenderFPS(TextRenderer & TextRenderer, Color color,int x, int y);
protected:
	float mDeltaTime;
	float mFrameRefreshTime;
	int mFrameCount;
	float mFPS;
};

