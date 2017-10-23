#include "FirstScene.hpp"
#include "SecondScene.hpp"
#include <memory>
#include <iomanip>
FirstScene::FirstScene()
{
}


FirstScene::~FirstScene()
{
}

void FirstScene::OnCreate()
{
	mUIBatches.Initialize(mFramework->GetGDI(), mBatches);
	wchar_t buffer[MAX_PATH];
	char cbuffer[MAX_PATH];
	Tools::GetInstance()->GetFontPath("Dengb.ttf", cbuffer, MAX_PATH);
	mAxisRenderer.Initialize(mFramework->GetGDI());
	
	InputType it[] = { SHADER_INPUTLAYOUT_POSITION, SHADER_INPUTLAYOUT_COLOR };
	mShader3D.Initialize(mFramework->GetGDI(), ShaderConst::shaderPCVS, ShaderConst::shaderPCVSSize, ShaderConst::shaderPCPS, ShaderConst::shaderPCPSSize, it, 2);
	mBatch3D.Initialize(mFramework->GetGDI(), &mShader3D, 100, 100);
	mCube.SetPositionAndSize(-1, -1, -1, 2, 2, 2);

	mFont.Initialize(mFramework->GetGDI(), cbuffer, 16);
    mFont_s.Initialize(mFramework->GetGDI(), cbuffer, 12);
	mFont_b.Initialize(mFramework->GetGDI(), cbuffer, 24);
	mTextRenderer.Initialize(mFramework->GetGDI(), &mFont, 1000);
    mTextRenderer_s.Initialize(mFramework->GetGDI(), &mFont_s, 1000);
	mTextRenderer_b.Initialize(mFramework->GetGDI(), &mFont_b, 1000);
	mFramework->AddInputListener(&mUILayer);
	mFramework->AddInputListener(this);
	AddLayer(&mUILayer);

	mLb.SetTextRenderer(&mTextRenderer_b);
	mLb.SetText(L"Direct3D11");
    mLb.SetPositionAndSize(50, 200, 150, 40);
	mLb.SetMouseEventable(true);

	mLxb.SetTextRenderer(&mTextRenderer);
	mLxb.SetText(L"X Game Render Framework! Click To Next Scene.");
    mLxb.SetPositionAndSize(0, 240, 200, 40);
	mLxb.SetMouseEventable(true);
	mLxb.SetClickable(true);
	mLxb.AddOnClickListener([=](const MousePoint& mp, int p) {
		SecondScene * sc = new SecondScene();
		SwitchScene(sc);
		DXGI_MODE_DESC * dc;
		int count = this->GetFramework()->GetGDI()->GetFullScreenDisplayModes(&dc);
		int i;
		for (i = 0; i < count; i++)
		{
			if (dc[i].Width == 800 && dc[i].Height == 600)
				break;
		}
		//this->GetFramework()->GetGDI()->SetFullScreenDisplayMode(i);
		//this->GetFramework()->GetGDI()->SetFullScreen(true);
	});
	mLb.SetMouseDowmListener([=](const MousePoint& mp, int p) {
		mLb.StartAction();
	});
	std::unique_ptr<Action> act;
	
	ActionBuilder::Builder()
	.BeginBuild()
		.ParallelActionTo()
			.SequenceActionTo()
				.MoveTo(180.f, 0.f, 0.f, 1.0f, AnticipateOvershootInterpolator::GetInterpolator(4))
				.DelayActionTo(0.2f)
				.RotateBy(0.f, 0.f, -DirectX::XM_PI * 2 , 1.0f, AnticipateOvershootInterpolator::GetInterpolator(2.5))
				.DelayActionTo(0.2f)
				.MoveTo(0.f, 180.f, 0.f, 1.4f, AnticipateOvershootInterpolator::GetInterpolator(4))
				.DelayActionTo(0.2f)
				.RotateBy(0.f, 0.f, -DirectX::XM_PI * 2, 1.0f, AnticipateOvershootInterpolator::GetInterpolator(2.5))
				.DelayActionTo(0.2f)
				.MoveTo(180.f, 180.f, 0.f, 1.4f, AnticipateOvershootInterpolator::GetInterpolator(4))
				.DelayActionTo(0.2f)
				.RotateBy(0.f, 0.f, -DirectX::XM_PI * 2, 1.0f, AnticipateOvershootInterpolator::GetInterpolator(2.5))
				.BackUp()
			.SequenceActionTo()
				.ChangeColorTo(1.f,1.f,0.f,1.f,1.f, AccelerateDecelerateInterpolator::GetInterpolator())
				.ChangeColorTo(1.f, 0.f, 1.f, 1.f, 1.f, AccelerateDecelerateInterpolator::GetInterpolator())
				.ChangeColorTo(0.f, 1.f, 1.f, 1.f, 1.f, AccelerateDecelerateInterpolator::GetInterpolator())
				.ChangeColorTo(1.f, 1.f, 1.f, 1.f, 1.f, AccelerateDecelerateInterpolator::GetInterpolator())
				.AlphaTo(0.f, 1.2f, AccelerateDecelerateInterpolator::GetInterpolator())
				.AlphaTo(1.f, 1.2f, AccelerateDecelerateInterpolator::GetInterpolator())
		.EndBuild(act);
	mLb.SetAction(std::move(act));
	
	mLb.SetRotationOrigin(Point(10,10,0));
    
	mBt.SetPositionAndSize(10,100,60,30);
	btNormal.LoadWIC(mFramework->GetGDI(), GetFilePath(L"a.png", buffer, 100));
    btMove.LoadWIC(mFramework->GetGDI(), GetFilePath(L"a1.png", buffer, 100));
    btPress.LoadWIC(mFramework->GetGDI(), GetFilePath(L"a2.png", buffer, 100));
	std::unique_ptr<Action> animbt;
	ActionBuilder::Builder().BeginBuild()
		.ParallelActionTo()
			.RotateBy(0, 0, DirectX::XM_2PI, 3.f, AnticipateOvershootInterpolator::GetInterpolator(2.5))
			.ScaleBy(0.1,0.1,0.1f,3.f, AnticipateOvershootInterpolator::GetInterpolator(2.5))
		.EndBuild(animbt);
	mBt.SetAction(std::move(animbt));
	mBt.SetTexture(&btNormal, &btMove, &btPress);
	mBt.AddOnClickListener([=](const MousePoint &mm, int pk) {
		mBt.StartAction();
		AsyncTask::NewTask(mFramework->GetTheard(), [](AsyncTask * asyn) {
			MessageBox(NULL, L"YOU CLICK BUTTOM!!",L"E",0);
			asyn->Finish(0, 0);
		});
		
    });
	mEdit.SetPositionAndSize(300, 20, 200, 40);
	mEdit.SetBorderSize(2);
	mEdit.SetTextRenderer(&mTextRenderer);

	mEdit2.SetPositionAndSize(100, 20, 200, 40);
	mEdit2.SetBorderSize(2);
	mEdit2.SetTextRenderer(&mTextRenderer);

	mUILayer.Add(&mEdit);
	mUILayer.Add(&mEdit2);
	mUILayer.Add(&mBt);
	mUILayer.Add(&mLxb);
	mUILayer.Add(&mLb);
	//GetFramework()->GetGDI()->ResizeTarget(500, 400); 
}

void FirstScene::OnDestory()
{
	mFramework->RemoveInputListener(this);
	mFramework->RemoveInputListener(&mUILayer);
	mUIBatches.Shutdown(mBatches);
    mTextRenderer_s.Shutdown();
	mTextRenderer.Shutdown();
	mTextRenderer_b.Shutdown();

	mFont.Shutdown();
	mFont_s.Shutdown();
	mFont_b.Shutdown();

	mAxisRenderer.Shutdown();
    btNormal.Release();
    btMove.Release();
    btPress.Release();

	mBatch3D.Shutdown();
	mShader3D.Shutdown();
	
}

void FirstScene::Render(float deltaTime)
{
	Clear(Color(0.5f, 0.5f, 0.5f, 0.0f));
	WVPMatrix wvp2D, wvp3D;
	mCamera2D.GetCameraMatrix(wvp2D);
	mCamera.GetCameraMatrix(wvp3D);
	
    auto debug = DebugInscriber::GetInstance();
	mTextRenderer_s.Begin(wvp2D);
	/*
	mTextRenderer_s.DrawStringWithNum(L"IndicesRenderCountPerFrame: ", debug->GetIndicesRenderCountPerFrame(), 2, mFramework->GetWindowsHeight() - 80);
	mTextRenderer_s.DrawStringWithNum(L"IndicesRenderCountPerSecond: ", debug->GetIndicesRenderCountPerSecond(), 300, mFramework->GetWindowsHeight() - 80);
	mTextRenderer_s.DrawStringWithNum(L"VerticesRenderCountPerFrame: ", debug->GetVerticesRenderCountPerFrame(), 2, mFramework->GetWindowsHeight() - 60);
	mTextRenderer_s.DrawStringWithNum(L"VerticesRenderCountPerSecond: ", debug->GetVerticesRenderCountPerSecond(), 300, mFramework->GetWindowsHeight() - 60);
	mTextRenderer_s.DrawStringWithNum(L"CallBatchPerFrame: ", debug->GetCallBatchPerFrame(), 2, mFramework->GetWindowsHeight() - 40);
	mTextRenderer_s.DrawStringWithNum(L"CallBatchPerSecond: ", debug->GetCallBatchPerSecond(), 300, mFramework->GetWindowsHeight() - 40);
	mTextRenderer_s.DrawStringWithNum(L"PolygonRenderCountPerFrame: ", debug->GetPolygonRenderCountPerFrame(), 2, mFramework->GetWindowsHeight() - 20);
	mTextRenderer_s.DrawStringWithNum(L"PolygonRenderCountPerSecond: ", debug->GetPolygonRenderCountPerSecond(), 300, mFramework->GetWindowsHeight() - 20);
	*/
	std::wstringstream str;
	str << std::fixed << std::setprecision(1);
	str << L"IndicesRenderCountPerFrame:" << debug->GetIndicesRenderCountPerFrame();
	mTextRenderer_s.DrawString(str.str().c_str(), 2, mFramework->GetWindowsHeight() - 80);
	str.clear();
	str.str(L"");
	str << L"IndicesRenderCountPerSecond:" << debug->GetIndicesRenderCountPerSecond();
	mTextRenderer_s.DrawString(str.str().c_str(), 300, mFramework->GetWindowsHeight() - 80);
	str.clear();
	str.str(L"");
	str << L"VerticesRenderCountPerFrame:" << debug->GetVerticesRenderCountPerFrame();
	mTextRenderer_s.DrawString(str.str().c_str(), 2, mFramework->GetWindowsHeight() - 60);
	str.clear();
	str.str(L"");
	str << L"VerticesRenderCountPerSecond:" << debug->GetVerticesRenderCountPerSecond();
	mTextRenderer_s.DrawString(str.str().c_str(), 300, mFramework->GetWindowsHeight() - 60);
	str.clear();
	str.str(L"");
	str << L"CallBatchPerFrame:" << debug->GetCallBatchPerFrame();
	mTextRenderer_s.DrawString(str.str().c_str(), 2, mFramework->GetWindowsHeight() - 40);
	str.clear();
	str.str(L"");
	str << L"CallBatchPerSecond:" << debug->GetCallBatchPerSecond();
	mTextRenderer_s.DrawString(str.str().c_str(), 300, mFramework->GetWindowsHeight() - 40);
	str.clear();
	str.str(L"");
	str << L"PolygonRenderCountPerSecond:" << debug->GetPolygonRenderCountPerSecond();
	mTextRenderer_s.DrawString(str.str().c_str(), 300, mFramework->GetWindowsHeight() - 20);
	str.clear();
	str.str(L"");
	str << L"PolygonRenderCountPerFrame:" << debug->GetPolygonRenderCountPerFrame();
	mTextRenderer_s.DrawString(str.str().c_str(), 2, mFramework->GetWindowsHeight() - 20);


	/*

    mTextRenderer_s.DrawStringEx(2,mFramework->GetWindowsHeight() - 80,L"IndicesRenderCountPerFrame: %.1f", debug->GetIndicesRenderCountPerFrame());
	mTextRenderer_s.DrawStringEx(300, mFramework->GetWindowsHeight() - 80, L"IndicesRenderCountPerSecond: %.1f", debug->GetIndicesRenderCountPerSecond());
    mTextRenderer_s.DrawStringEx(2, mFramework->GetWindowsHeight() - 60, L"VerticesRenderCountPerFrame: %.1f", debug->GetVerticesRenderCountPerFrame());
	mTextRenderer_s.DrawStringEx(300, mFramework->GetWindowsHeight() - 60, L"VerticesRenderCountPerSecond: %.1f", debug->GetVerticesRenderCountPerSecond());
    mTextRenderer_s.DrawStringEx(2, mFramework->GetWindowsHeight() - 40, L"CallBatchPerFrame: %.1f", debug->GetCallBatchPerFrame());
    mTextRenderer_s.DrawStringEx(300, mFramework->GetWindowsHeight() - 40, L"CallBatchPerSecond: %.1f", debug->GetCallBatchPerSecond());
    mTextRenderer_s.DrawStringEx(2, mFramework->GetWindowsHeight() - 20, L"PolygonRenderCountPerFrame: %.1f", debug->GetPolygonRenderCountPerFrame());
    mTextRenderer_s.DrawStringEx(300, mFramework->GetWindowsHeight() - 20, L"PolygonRenderCountPerSecond: %.1f", debug->GetPolygonRenderCountPerSecond());
	*/
	mTextRenderer_s.End();
	
	mAxisRenderer.Begin(wvp3D);
	mAxisRenderer.SetAxisXColor(Color(1.f, 0.f, 0.f, 1.0), Color(0.5f, 0.f, 0.f, 1.f));
	mAxisRenderer.SetAxisYColor(Color(0.f, 1.f, 0.f, 1.0), Color(0.f, 0.5f, 0.f, 1.f));
	mAxisRenderer.SetAxisZColor(Color(0.f, 0.f, 1.f, 1.0), Color(0.f, 0.f, 0.5f, 1.f));
	mAxisRenderer.DrawAxis();
	mAxisRenderer.End();

	
	PolygonPleColorBinder cb(mCube.mPolygon.mCount);
	cb.mColor[0] = Color(1.0, 0.0, 0.0, 1.0);
	cb.mColor[1] = Color(1.0, 1.0, 0.0, 1.0);
	cb.mColor[2] = Color(1.0, 1.0, 1.0, 1.0);
	cb.mColor[3] = Color(0.0, 1.0, 1.0, 1.0);
	cb.mColor[4] = Color(0.0, 1.0, 0.0, 1.0);
	cb.mColor[5] = Color(0.0, 0.0, 1.0, 1.0);
	cb.mColor[6] = Color(0.0, 0.0, 0.0, 1.0);
	cb.mColor[7] = Color(1.0, 0.0, 1.0, 1.0);
	BindingBridge bbr;
	bbr.AddBinder(cb);
	mBatch3D.Begin(wvp3D);
	mBatch3D.DrawPolygon(mCube.mPolygon, mCube.mPolygonPleIndex, bbr);
	mBatch3D.End();
	
	mUIBatches.Begin();

	mTextRenderer_b.Begin(wvp2D);
	mTextRenderer.Begin(wvp2D);
	
	UpdataLayer(deltaTime);
	RenderLayer(mBatches);
	str.clear();
	str.str(L"");
	str << L"FPS:" << debug->GetAverageFPS();
	mTextRenderer.DrawString(str.str().c_str(), 4, 4);
	mTextRenderer.End();
	mTextRenderer_b.End();
	mUIBatches.End();
}

void FirstScene::Updata(float deltaTime)
{
    mCamera.Updata();
    mCamera2D.Updata();
	auto ip = GetFramework()->GetInputManager();
	if (ip->IskeyDowm(DIK_ESCAPE))
		GetFramework()->Exit(0);
}

void FirstScene::OnSize(int ClientX, int ClientY)
{
	mCamera2D.UpdataSize(ClientX,ClientY);
	mCamera.UpdataSize(ClientX, ClientY);
	mUIBatches.OnSize(ClientX, ClientY);
}

void FirstScene::OnActivate(bool isActivate)
{
}

void FirstScene::OnMouseMove(const MousePoint & mm, int pk)
{
	float h = GetFramework()->GetWindowsHeight();
	float w = GetFramework()->GetWindowsWidth();
	mCamera.PitchYawRoll(mCamera.GetFovAngle()/w, mCamera.GetFovAngle()/h,0);
}

void FirstScene::OnKeyDowm(Key k)
{
	switch (k)
	{
		case DIK_W:
			mCamera.Walk(0.1f);
			break;
		case DIK_S:
			mCamera.Walk(-0.1f);
			break;
		case DIK_A:
			mCamera.Strafe(0.1f);
			break;
		case DIK_D:
			mCamera.Strafe(-0.1f);
			break;
		case DIK_Z:
			mCamera.Fly(-0.1f);
			break;
		case DIK_X:
			mCamera.Fly(0.1f);
			break;
		case DIK_T:
			mCamera.Roll(0.1f);
			break;
		case DIK_G:
			mCamera.Roll(-0.1f);
			break;
		case DIK_R:
			mCamera.Pitch(0.1f);
			break;
		case DIK_Y:
			mCamera.Pitch(-0.1f);
			break;
		case DIK_F:
			mCamera.Yaw(0.1f);
			break;
		case DIK_H:
			mCamera.Yaw(-0.1f);
			break;
	default:
		break;
	}
	
}
