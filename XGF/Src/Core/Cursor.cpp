#include "..\..\Include\Cursor.hpp"
#include "..\..\Include\ConstantData.hpp"


Cursor::Cursor():mPtBinder(4)
{
}


Cursor::~Cursor()
{
}

void Cursor::Initialize(GDI * gdi)
{
	mTextureBatch.Initialize(gdi, ConstantData::GetInstance().GetPTShader(), 4, 6);
	mBbrg.AddBinder(mPtBinder);
	mIsShow = false;
}

void Cursor::Shutdown()
{
	mTextureBatch.Shutdown();
}

void Cursor::Draw(const WVPMatrix & wvp)
{
	if (!mIsShow)  return ;
	mTextureBatch.Begin(wvp);
	mRc.Render(mTextureBatch, nullptr, mBbrg, mTexture.GetShaderResourceView());
	mTextureBatch.End();
}

void Cursor::SetPosition(int x, int y)
{
	mx = x;
	my = y;

}
