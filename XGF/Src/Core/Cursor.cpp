#include "..\..\Include\Cursor.hpp"
#include "../../Include/ShaderManager.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/Renderer.hpp"

namespace XGF
{
	Cursor::Cursor() :mPtBinder(std::make_shared<PolygonPleTextureBinder>(4)), mIsShow(false), mUsedStaticTexture(true), mAnimation(0), mPointDeviation(0.f, 0.f), mPassTime(0.f), mTexture(nullptr)
	{
	}

	Cursor::~Cursor()
	{
	}

	void Cursor::Initialize()
	{
		mShaderStage.Initialize(Context::Current().QueryShaderManager().GetBasicShaders(false, true, false));
		mShaderStage.SetBlendState(BlendState::AddOneOneAdd);
		mShaderStage.SetDepthStencilState(DepthStencilState::DepthDisable);
		mBbrg.AddBinder(mRc.mPolygon);
		mBbrg.AddBinder(mPtBinder);
		mSize.x = 16;
		mSize.y = 16;
		mIsShow = false;
	}

	void Cursor::Shutdown()
	{
		mShaderStage.Shutdown();
	}

	void Cursor::Draw(const WVPMatrix & wvp)
	{
		mShaderStage.SetPSConstantBuffer(0, Color(1.f, 1.f, 1.f, 1.f));
		if (mUsedStaticTexture)
		{
			if (!mIsShow || mTexture == nullptr)  return;
			mShaderStage.SetVSConstantBuffer(0, &wvp);
			mRc.SetPositionAndSize(mPosition.x - mPointDeviation.x, mPosition.y - mPointDeviation.y, mSize.x, mSize.y);

			mPtBinder->GetData(1).x = mPtBinder->GetData(0).x = 0.f;
			mPtBinder->GetData(2).x = mPtBinder->GetData(3).x = 1.f;
			mPtBinder->GetData(1).y = mPtBinder->GetData(2).y = 1.f;
			mPtBinder->GetData(3).y = mPtBinder->GetData(0).y = 0.f;

			mRc.SetZ(0.1f);
			mShaderStage.SetPSTexture(0, mTexture);
			Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(mBbrg, *mRc.mPolygonPleIndex.get(), mShaderStage));
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

			mShaderStage.SetVSConstantBuffer(0, &wvp);
			mRc.SetPositionAndSize(mPosition.x - mPointDeviation.x, mPosition.y - mPointDeviation.y, mSize.x, mSize.y);
			mRc.SetZ(0.1f);
			mShaderStage.SetPSTexture(0, mTexture);
			Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(mBbrg, *mRc.mPolygonPleIndex.get(), mShaderStage));
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

	void Cursor::SetStaticTexture(Texture * texture)
	{
		mTexture = texture;
		mUsedStaticTexture = texture != nullptr;
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
