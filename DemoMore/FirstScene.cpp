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
	wchar_t buffer[MAX_PATH];
	char cbuffer[MAX_PATH];
	Tools::GetInstance()->GetFontPath("Dengb.ttf", cbuffer, MAX_PATH);
	mAxisRenderer.Initialize(mFramework->GetGDI());
	
	mBatch3D.Initialize(mFramework->GetGDI(), ConstantData::GetInstance().GetPCShaders(), 100, 100);
	mCube.SetPositionAndSize(-1, -1, -1, 2, 2, 2);

	mFont.Initialize(mFramework->GetGDI(), cbuffer, 16);
    mFont_s.Initialize(mFramework->GetGDI(), cbuffer, 12);
	mFont_b.Initialize(mFramework->GetGDI(), cbuffer, 20);
	mUITextRenderer.Initialize(mFramework->GetGDI(), &mFont, 1000);
    mTextRenderer_s.Initialize(mFramework->GetGDI(), &mFont_s, 1000);
	mTextRenderer_b.Initialize(mFramework->GetGDI(), &mFont_b, 200);
	mFramework->AddInputListener(&mUILayer);
	mFramework->AddInputListener(this);
	AddLayer(&mUILayer);

	mLb.SetText(L"Direct3D11");
    mLb.SetPositionAndSize(50, 200, 150, 40);
	mLb.SetMouseEventable(true);
	mLb.SetZ(0.05f);

	mLxb.SetText(L"X Game Render Framework\nClick here switch next scene.");
    mLxb.SetPositionAndSize(0, 240, 240, 40);
	mLxb.SetMouseEventable(true);
	mLxb.SetClickable(true);
	mLxb.AddOnClickListener([=](const MousePoint& mp, int p) {
		SecondScene * sc = new SecondScene();
		SwitchScene(sc);
	});
	mLxb.SetZ(0.06f);
	mLb.SetMouseDowmListener([=](const MousePoint& mp, int p) {
		mLb.StartAction();
	});
	std::unique_ptr<Action> act;
	
	ActionBuilder::Builder()
	.BeginBuild()
		.ParallelActionTo()
			.SequenceActionTo()
				.MoveTo(180.f, 0.f, 0.4f, 1.0f, AnticipateOvershootInterpolator::GetInterpolator(4))
				.DelayActionTo(0.2f)
				.RotateBy(0.f, 0.f, -DirectX::XM_PI * 2 , 1.0f, AnticipateOvershootInterpolator::GetInterpolator(2.5))
				.DelayActionTo(0.2f)
				.MoveTo(0.f, 180.f, 0.4f, 1.4f, AnticipateOvershootInterpolator::GetInterpolator(4))
				.DelayActionTo(0.2f)
				.RotateBy(0.f, 0.f, -DirectX::XM_PI * 2, 1.0f, AnticipateOvershootInterpolator::GetInterpolator(2.5))
				.DelayActionTo(0.2f)
				.MoveTo(180.f, 180.f, 0.4f, 1.4f, AnticipateOvershootInterpolator::GetInterpolator(4))
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
			.ScaleBy(0.1f,0.1f,1.0f,3.f, AnticipateOvershootInterpolator::GetInterpolator(2.5))
		.EndBuild(animbt);
	mBt.SetAction(std::move(animbt));
	mBt.SetTexture(&btNormal, &btMove, &btPress);
	mBt.AddOnClickListener([=](const MousePoint &mm, int pk) {
		mBt.StartAction();
		if (mFramework->GetGDI()->GetDisplayMode() == DisplayMode::Borderless)
			mFramework->GetGDI()->SetDisplayMode(DisplayMode::Windowed, 0, 0, 600, 400, false);
		else if(mFramework->GetGDI()->GetDisplayMode() == DisplayMode::FullScreen)
			mFramework->GetGDI()->SetDisplayMode(DisplayMode::Borderless ,0 ,0 , 1920, 1080, true);
		else
			mFramework->GetGDI()->SetDisplayMode(DisplayMode::FullScreen, 0, 0, 1920, 1080, true);
		//AsyncTask::NewTask(mFramework->GetTheard(), [this](AsyncTask * asyn) {
			
			//MessageBox(NULL, L"YOU CLICK BUTTOM!!",L"E",0);
			//asyn->Finish(0, 0);
		//});
		
    });
	mBt.SetZ(0.07f);
	mEdit.SetPositionAndSize(300, 20, 200, 40);
	mEdit.SetBorderSize(2);
	mEdit.SetZ(0.08f);

	mEdit2.SetPositionAndSize(140, 20, 120, 40);
	mEdit2.SetBorderSize(2);
	mEdit2.SetZ(0.08f);

	mUILayer.Add(&mEdit);
	mUILayer.Add(&mEdit2);
	mUILayer.Add(&mBt);
	mUILayer.Add(&mLxb);
	mUILayer.Add(&mLb);
	mUILayer.GetUIBatches()->SetTextRenderer(BATCHES_TEXTRENDERER_DEFAULT_SIZE, &mUITextRenderer);
	mFramework->GetInputManager()->GetCursor()->SetStaticTexture(mFramework->GetGDI(), GetFilePath(L"cursor.png", buffer, 100));
	std::unique_ptr<Action> sceneact;

	ActionBuilder::Builder()
		.BeginBuild()
			.ParallelActionTo()
				.SequenceActionTo()
					.MoveTo(0.f, 0.f, 0.0f, 2.0f, AnticipateOvershootInterpolator::GetInterpolator(4))
					.BackUp()
				.SequenceActionTo()
					.ChangeColorTo(1.f, 1.f, 1.f, 1.f, 2.f, LinearInterpolator::GetInterpolator())
		.EndBuild(sceneact);
	mSceneAnimationIn.OnPositionChange(Point(-500.f, 0.f, 0.f), 0);
	mSceneAnimationIn.OnColorChange(Color(0.0, 0.0, 0.0, 0.f),0);
	mSceneAnimationIn.SetAction(std::move(sceneact));
	
	std::unique_ptr<Action> sceneact2;

	ActionBuilder::Builder()
		.BeginBuild()
		.ChangeColorTo(1.f, 1.f, 1.f, 0.f, 2.f, LinearInterpolator::GetInterpolator())
		.EndBuild(sceneact2);
	mSceneAnimationOut.OnColorChange(Color(1.0, 1.0, 1.0, 1.f),0);
	mSceneAnimationOut.SetAction(std::move(sceneact2));
}

void FirstScene::OnDestory()
{
	Scene::OnDestory();
	mFramework->RemoveInputListener(this);
	mFramework->RemoveInputListener(&mUILayer);
    mTextRenderer_s.Shutdown();
	mUITextRenderer.Shutdown();
	mTextRenderer_b.Shutdown();

	mFont.Shutdown();
	mFont_s.Shutdown();
	mFont_b.Shutdown();

	mAxisRenderer.Shutdown();
    btNormal.Release();
    btMove.Release();
    btPress.Release();

	mBatch3D.Shutdown();
}

void FirstScene::Render(float deltaTime)
{
	Clear(Color(0.5f, 0.5f, 0.5f, 0.0f));
	WVPMatrix wvp2D, wvp3D;
	mCamera2D.GetCameraMatrix(wvp2D);
	mCamera.GetCameraMatrix(wvp3D);
	
    auto debug = DebugInscriber::GetInstance();
	mTextRenderer_s.Begin(wvp2D);

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
	bbr.AddBinder(mCube.mPolygon);
	bbr.AddBinder(cb);
	mBatch3D.GetShaderStage()->SetVSConstantBuffer(0, &wvp3D);
	mBatch3D.Begin();
	mBatch3D.DrawPolygon(mCube.mPolygonPleIndex, bbr);
	mBatch3D.End();
	

	mTextRenderer_b.Begin(wvp2D);
	
	UpdataLayer(deltaTime);
	RenderLayer(wvp2D);
	str.clear();
	str.str(L"");
	str << std::fixed << std::setprecision(1) << L"FPS:" << debug->GetAverageFPS() << "\n" << L"FC:" << std::setprecision(4) << debug->GetFrameCost() << "ms";
	mTextRenderer_b.DrawString(str.str().c_str(), Color(0.2f,0.2f,0.8f,1.0f),4, 4);
	mTextRenderer_b.End();
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
	mCamera2D.UpdataProject(ClientX, ClientY);
	mCamera.UpdataProject(ClientX, ClientY);
}

void FirstScene::OnActivate(bool isActivate)
{
}

SceneAnimation * FirstScene::OnSwitchIn()
{
	return &mSceneAnimationIn;
}

SceneAnimation * FirstScene::OnSwitchOut()
{
	return &mSceneAnimationOut;
}

void FirstScene::OnMouseMove(const MousePoint & mm, int pk)
{
	float h = GetFramework()->GetWindowsHeight();
	float w = GetFramework()->GetWindowsWidth();
	mCamera.Pitch(mCamera.GetFovAngle() / h);
	mCamera.Yaw(mCamera.GetFovAngle() / w);
}

void FirstScene::OnKeyDowm(Key k)
{
	switch (k)
	{
	case DIK_W:
		mCamera.Walk(1);
		break;
	case DIK_S:
		mCamera.Walk(-1);
		break;
	case DIK_A:
		mCamera.Strafe(-1);
		break;
	case DIK_D:
		mCamera.Strafe(1);
		break;
	case DIK_Z:
		mCamera.Fly(-1);
		break;
	case DIK_X:
		mCamera.Fly(1);
		break;
	case DIK_R:
		mCamera.Pitch(1);
		break;
	case DIK_Y:
		mCamera.Pitch(-1);
		break;
	case DIK_F:
		mCamera.Yaw(1);
		break;
	case DIK_H:
		mCamera.Yaw(-1);
		break;
	default:
		break;
	}
	
}
