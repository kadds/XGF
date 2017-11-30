#include "..\..\Include\Cursor.hpp"
#include "..\..\Include\ConstantData.hpp"

Cursor::Cursor() :mPtBinder(4), mIsShow(false), mUsedStaticTexture(true),mAnimation(0), mPointDeviation(0.f,0.f), mPassTime(0.f)
{
}

Cursor::~Cursor()
{
}

void Cursor::Initialize(GDI * gdi)
{
	mTextureBatch.Initialize(gdi, ConstantData::GetInstance().GetPTShader(), 4, 6);
	mTextureBatch.SetBlend(true);
	mTextureBatch.SetZBufferRender(false);
	mBbrg.AddBinder(mPtBinder);
	
	mSize.x = 16;
	mSize.y = 16;
	mIsShow = false;
}

void Cursor::Shutdown()
{
	mTexture.Release();
	mTextureBatch.Shutdown();
}

void Cursor::Draw(const WVPMatrix & wvp)
{
	if (mUsedStaticTexture)
	{
		if (!mIsShow || mTexture.GetShaderResourceView() == nullptr)  return;
		mTextureBatch.Begin(wvp);
		mRc.SetPositionAndSize(mPosition.x - mPointDeviation.x, mPosition.y - mPointDeviation.y, mSize.x, mSize.y);
		mPtBinder.FromTexture(&mTexture);
		mRc.SetZ(0.f);
		mRc.Render(mTextureBatch, nullptr, mBbrg, mTexture.GetShaderResourceView());
		mTextureBatch.End();
	}
	else
	{
		if (!mIsShow)  return;
		BindingBridge bbr;
		bbr.AddBinder(mPtBinder);
		float x, y, w, h;
		mAnimation->GetPositionInTexture(x,y,w,h);
		mPtBinder.SetPosition(x, w, y, h);
		mTextureBatch.Begin(wvp);
		mRc.SetPositionAndSize(mPosition.x - mPointDeviation.x, mPosition.y - mPointDeviation.y, mSize.x, mSize.y);
		mRc.SetZ(0.f);
		mRc.Render(mTextureBatch, nullptr, mBbrg, mAnimation->GetTexture()->GetShaderResourceView());
		mTextureBatch.End();
	}
}

void Cursor::SetPosition(float x, float y)
{
	mPosition.x = x;
	mPosition.y = y;

}

void Cursor::Show()
{
	mIsShow = true; 
}

void Cursor::Tick(float time)
{
	mPassTime += time;
	if (!mUsedStaticTexture)
	{
		mAnimation->Tick(time);
	}
}

void Cursor::SetStaticTexture(GDI * gdi, const wchar_t * t)
{
	mTexture.LoadWIC(gdi, t);
	mUsedStaticTexture = true;
}

void Cursor::SetAnimation(Animation * mm)
{
	if (mAnimation == nullptr) return;
	mAnimation = mm;
	mUsedStaticTexture = false;
}

void Cursor::Hide() {
	mIsShow = false;
}

