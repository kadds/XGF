#include "..\..\Include\Caret.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/ShaderManager.hpp"
#include "../../Include/Renderer.hpp"

namespace XGF
{
	Caret::Caret() 
	{
		mPcColor = std::make_shared<PolygonPleConstantColorBinder>(4, mCaretColor);
	}


	Caret::~Caret()
	{
	}

	void Caret::Initialize()
	{
		mIsCaretShow = false;
		mShaderStage.Initialize(Context::Current().QueryShaderManager().GetBasicShaders(false, false, true));
		mShaderStage.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		mShaderStage.SetBlendState(BlendState::AddOneOneAdd);
		mShaderStage.SetDepthStencilState(DepthStencilState::DepthDisable);
		
		mHide = false;
		mHeight = 16;
		mBbrg.AddBinder(mLine.mPolygon);
		mBbrg.AddBinder(mPcColor);
		mPcColor->GetData(0) = Color(0.f, 0.f, 0.f, 1.f);
	}

	void Caret::Shutdown()
	{
		mShaderStage.Shutdown();
	}

	void Caret::Draw(const WVPMatrix & wvp)
	{
		if (!mIsCaretShow || mHide)
		{
			return;
		}
		mShaderStage.SetVSConstantBuffer(0, &wvp);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(mBbrg, *mLine.mPolygonPleIndex.get(), mShaderStage));

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

