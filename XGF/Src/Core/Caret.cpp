#include "..\..\Include\Caret.hpp"
#include "..\..\Include\ConstantData.hpp"


Caret::Caret():mPcColor(mCaretColor, 4)
{
}


Caret::~Caret()
{
}

void Caret::Initialize(GDI * gdi)
{
	mIsCaretShow = false;
	mColorBatch.Initialize(gdi, ConstantData::GetInstance().GetPCShader(), 16, 16, TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	mColorBatch.SetBlend(true);
	mColorBatch.SetZBufferRender(false);
	mHide = false;
	mHeight = 16;
	mBbrg.AddBinder(mPcColor);
	mPcColor.Set(0, 1, Color(0.f, 0.f, 0.f, 1.f));
}

void Caret::Shutdown()
{
	mColorBatch.Shutdown();
}

void Caret::Draw(const WVPMatrix & wvp)
{
	if (!mIsCaretShow || mHide)
	{
		return ;
	}
	mColorBatch.Begin(wvp);
	mColorBatch.SetTexture(nullptr);
	mColorBatch.DrawPolygon(mLine.mPolygon, mLine.mPolygonPleIndex, mBbrg);
	mColorBatch.End();
}

void Caret::Tick(float time)
{
	mTime += time;
	if (mTime >= 1.0f)
	{
		mTime = 0.f;
		mHide = !mHide;
	}
}

void Caret::SetPosition(int x, int y)
{
	mLine.SetPosition(Point(x, y, 0.0f));
	mLine.SetEndPosition(Point(x, y + mHeight, 0.f));
	mLine.mPolygon.Transform(Batch::GetClientWidthD2(), Batch::GetClientHeightD2());
}