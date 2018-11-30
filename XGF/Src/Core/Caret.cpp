#include "..\..\Include\Caret.hpp"
#include "..\..\Include\ConstantData.hpp"

namespace XGF
{
	Caret::Caret() 
	{
		mPcColor = std::make_shared<PolygonPleConstantColorBinder>(4, mCaretColor);
	}


	Caret::~Caret()
	{
	}

	void Caret::Initialize(GDI * gdi)
	{
		mIsCaretShow = false;
		mColorBatch.Initialize(gdi, ConstantData::GetInstance().GetPCShaders(), 16, 16, TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		mColorBatch.GetShaderStage()->SetBlendState(BlendState::AddOneOneAdd);
		mColorBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthDisable);
		
		mHide = false;
		mHeight = 16;
		mBbrg.AddBinder(mLine.mPolygon);
		mBbrg.AddBinder(mPcColor);
		mPcColor->GetData(0) = Color(0.f, 0.f, 0.f, 1.f);
	}

	void Caret::Shutdown()
	{
		mColorBatch.Shutdown();
	}

	void Caret::Draw(const WVPMatrix & wvp)
	{
		if (!mIsCaretShow || mHide)
		{
			return;
		}
		mColorBatch.GetShaderStage()->SetVSConstantBuffer(0, &wvp);
		mColorBatch.Begin();
		mColorBatch.DrawPolygon(mLine.mPolygonPleIndex, mBbrg);
		mColorBatch.End();
	}

	void Caret::Tick(float time)
	{
		mTime += time;
		if (mTime >= 0.7f)
		{
			mTime = 0.f;
			mHide = !mHide;
		}
	}

	void Caret::SetPosition(float x, float y)
	{
		mLine.SetPosition(Point(x, y, 0.1f));
		mLine.SetEndPosition(Point(x, y + mHeight, 0.1f));
	}

	void Caret::ResetTime()
	{
		mTime = 0.f;
		mHide = false;
	}

}

