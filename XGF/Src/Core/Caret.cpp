#include "..\..\Include\Caret.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/ShaderManager.hpp"
#include "../../Include/Renderer.hpp"
#include "../../Include/SystemShaders.hpp"

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
		mRenderResource.ReCreate(SystemShaders::GetBasicShaders(SystemShaders::BasicShader_VertexColor));
		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		mRenderState.GetDepthStencilState().SetDepthEnable(false);
		
		mHide = false;
		mHeight = 16;
		mBbrg.AddBinder(mLine.mPolygon);
		mBbrg.AddBinder(mPcColor);
		mPcColor->GetData(0) = Color(0.f, 0.f, 0.f, 1.f);
	}

	void Caret::Shutdown()
	{
		mRenderResource.Clear();
	}

	void Caret::Draw(const WVPMatrix & wvp)
	{
		if (!mIsCaretShow || mHide)
		{
			return;
		}
		mRenderResource.SetConstantBuffer<VertexShader>(0, 0, wvp);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(mBbrg, *mLine.mPolygonPleIndex.get(), RenderStage(mRenderState, mRenderResource)));

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

