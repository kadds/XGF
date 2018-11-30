#include "..\..\Include\Cursor.hpp"
#include "..\..\Include\ConstantData.hpp"
namespace XGF
{
	Cursor::Cursor() :mPtBinder(std::make_shared<PolygonPleTextureBinder>(4)), mIsShow(false), mUsedStaticTexture(true), mAnimation(0), mPointDeviation(0.f, 0.f), mPassTime(0.f), mTextureResource(nullptr)
	{
	}

	Cursor::~Cursor()
	{
	}

	void Cursor::Initialize(GDI * gdi)
	{
		mTextureBatch.Initialize(gdi, ConstantData::GetInstance().GetPTShaders(), 4, 6);
		mTextureBatch.GetShaderStage()->SetBlendState(BlendState::AddOneOneAdd);
		mTextureBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthDisable);
		mBbrg.AddBinder(mPtBinder);
		mSize.x = 16;
		mSize.y = 16;
		mIsShow = false;
	}

	void Cursor::Shutdown()
	{
		mTextureBatch.Shutdown();
	}

	void Cursor::Draw(const WVPMatrix & wvp)
	{
		if (mUsedStaticTexture)
		{
			if (!mIsShow || mTextureResource == nullptr || mTexture.GetRawTexture() == nullptr)  return;
			mTextureBatch.GetShaderStage()->SetVSConstantBuffer(0, &wvp);
			mTextureBatch.Begin();
			mRc.SetPositionAndSize(mPosition.x - mPointDeviation.x, mPosition.y - mPointDeviation.y, mSize.x, mSize.y);
			
			mPtBinder->GetData(0).x = mPtBinder->GetData(3).x = 0.f;
			mPtBinder->GetData(0).y = mPtBinder->GetData(1).y = 0.f;
			mPtBinder->GetData(1).x = mPtBinder->GetData(2).x = 1.f;
			mPtBinder->GetData(2).y = mPtBinder->GetData(3).y = 1.f;

			mRc.SetZ(0.1f);
			mTextureBatch.GetShaderStage()->SetPSSRV(0, mTexture.GetRawTexture());
			mTextureBatch.DrawPolygon(mRc.mPolygonPleIndex, mBbrg);
			mTextureBatch.End();
		}
		else
		{
			if (!mIsShow)  return;
			BindingBridge bbr;
			bbr.AddBinder(mPtBinder);
			float x, y, w, h;
			mAnimation->GetPositionInTexture(x, y, w, h);
			mPtBinder->GetData(0).x = mPtBinder->GetData(3).x = x;
			mPtBinder->GetData(0).y = mPtBinder->GetData(1).y = y;
			mPtBinder->GetData(1).x = mPtBinder->GetData(2).x = x + w;
			mPtBinder->GetData(2).y = mPtBinder->GetData(3).y = y + h;

			mTextureBatch.GetShaderStage()->SetVSConstantBuffer(0, &wvp);
			mTextureBatch.Begin();
			mRc.SetPositionAndSize(mPosition.x - mPointDeviation.x, mPosition.y - mPointDeviation.y, mSize.x, mSize.y);
			mRc.SetZ(0.1f);
			mTextureBatch.GetShaderStage()->SetPSSRV(0, mTexture.GetRawTexture());
			mTextureBatch.DrawPolygon(mRc.mPolygonPleIndex, mBbrg);
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

	void Cursor::SetStaticTexture(TextureResource * tres)
	{
		mTextureResource = tres;
		mTexture.SetTextureResource(tres);
		mUsedStaticTexture = true;
	}

	void Cursor::SetAnimation(Animation * mm)
	{
		mAnimation = mm;
		mUsedStaticTexture = false;
	}

	void Cursor::Hide() {
		mIsShow = false;
	}


}
