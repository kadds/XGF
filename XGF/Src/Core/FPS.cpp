#include "../../Include/FPS.hpp"
#include "../../Include/TextRenderer.hpp"

namespace XGF
{
	FPS::FPS()
	{
		mFrameCount = 0;
		mFrameRefreshTime = 0;
		mFPS = 0;
	}


	FPS::~FPS()
	{
	}

	void FPS::Tick(float delat)
	{
		mFrameCount++;
		mFrameRefreshTime += delat;
		if (mFrameRefreshTime > 1.00f)
		{
			mFPS = mFrameCount / mFrameRefreshTime;
			mFrameRefreshTime = 0.0f;
			mFrameCount = 0;
			//OutputDebugStringEx(L"FPS:%f\n", mFPS);
		}
	}

	void FPS::RenderFPS(TextRenderer & TextRenderer)
	{
		RenderFPS(TextRenderer, 2, 4);
	}

	void FPS::RenderFPS(TextRenderer & TextRenderer, int x, int y)
	{
		wchar_t fps[30];
		swprintf(fps, 29, L"FPS:%.2f", mFPS);
		TextRenderer.DrawString(fps, static_cast<float>(x), static_cast<float>(y));
	}

	void FPS::RenderFPS(TextRenderer & TextRenderer, SM::Color color, int x, int y)
	{
		wchar_t fps[30];
		swprintf(fps, 29, L"FPS:%.2f", mFPS);
		TextRenderer.DrawString(fps, color, static_cast<float>(x), static_cast<float>(y));
	}

}
