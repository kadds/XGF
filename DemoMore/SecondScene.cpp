#include "SecondScene.hpp"
#include <iomanip>

SecondScene::SecondScene():mAnimation(0)
{
}


SecondScene::~SecondScene()
{
}

void SecondScene::OnCreate()
{
	char cbuffer[MAX_PATH];
	wchar_t buffer[MAX_PATH];
	GetFramework()->AddInputListener(&mUILayer);
	AddLayer(&mUILayer);
	mLabel.SetPositionAndSize(20, 60, 100, 40);
	mLabel.SetText(L"第二个界面 按Esc退出");
	mUILayer.Add(&mLabel);
	//mCamera3D.FixYAxis(true);
	mLabel2.SetPositionAndSize(20, 100, 100, 40);
	mLabel2.SetText(L"Activate!");
	mLabel2.SetClickable(true);
	mLabel2.SetMouseEventable(true);
	mLabel2.AddOnClickListener([this](const MousePoint & ms, int pk)
	{
		mLabel2.SetText(L"Click!");
		//this->GetFramework()->GetGDI()->SetFullScreen(false);
	});
	mUILayer.Add(&mLabel2);
	Tools::GetInstance()->GetFontPath("Dengb.ttf", cbuffer, MAX_PATH);
	mFont.Initialize(mFramework->GetGDI(), cbuffer, 16);
	mTextRenderer.Initialize(mFramework->GetGDI(), &mFont, 140);
	mUITextRenderer.Initialize(mFramework->GetGDI(), &mFont, 140);

	mGridRender.Initialize(mFramework->GetGDI(), 10, 10, 5, 5);
	mAxisRenderer.Initialize(mFramework->GetGDI());

	mCursorTexture.LoadWIC(mFramework->GetGDI(), GetFilePath(L"tcursor.png", buffer, MAX_PATH));
	mUILayer.GetUIBatches()->SetTextRenderer(BATCHES_TEXTRENDERER_DEFAULT_SIZE, &mUITextRenderer);
	AnimationStage stage[4];
	float c = 34.0 / 64.0;
	float b = 32.0 / 64.0;
	stage[0].left = 0.f;
	stage[1].left = c;
	stage[2].left = 0.f;
	stage[3].left = c;
	stage[0].top = 0.f;
	stage[1].top = 0.f;
	stage[2].top = c;
	stage[3].top = c;
	stage[0].right = b;
	stage[1].right = 1.0f;
	stage[2].right = b;
	stage[3].right = 1.0f;
	stage[0].bottom = b;
	stage[1].bottom = b;
	stage[2].bottom = 1.0f;
	stage[3].bottom = 1.0f;
	mAnimation.SetAnimation(0, 0.01f, 0.f, mCursorTexture, stage, 4);
	GetFramework()->GetInputManager()->GetCursor()->SetAnimation(&mAnimation);
	GetFramework()->GetInputManager()->GetCursor()->SetPointDeviation(Position(3.f, 0.f));
	GetFramework()->GetInputManager()->SetMouseMode(MouseMode::CustomCenter);
	mFramework->AddInputListener(this);
	std::unique_ptr<Action> sceneact;
	ActionBuilder::Builder()
		.BeginBuild()
		.ChangeColorTo(1.f, 1.f, 1.f, 1.f, 2.f, LinearInterpolator::GetInterpolator())
		.EndBuild(sceneact);
	mSceneAnimationIn.OnColorChange(Color(1.0, 1.0, 1.0, 0.f), 0);
	mSceneAnimationIn.SetAction(std::move(sceneact));
}
void SecondScene::OnDestory()
{
	Scene::OnDestory();
	mTextRenderer.Shutdown();
	mFont.Shutdown();
	mCursorTexture.Release();
	mAxisRenderer.Shutdown();
	mUITextRenderer.Shutdown();
	mGridRender.Shutdown();
	GetFramework()->RemoveInputListener(&mUILayer);
}

void SecondScene::Render(float deltaTime)
{
	Clear(Color(0.5f, 0.5f, 0.5f, 0.0f));
	WVPMatrix wvp, wvp3D;
	mCamera.GetCameraMatrix(wvp);
	mCamera3D.GetCameraMatrix(wvp3D);
	auto debug = DebugInscriber::GetInstance();
	mAxisRenderer.Begin(wvp3D);
	mAxisRenderer.SetAxisXColor(Color(1.f, 0.f, 0.f, 1.0), Color(0.5f, 0.f, 0.f, 1.f));
	mAxisRenderer.SetAxisYColor(Color(0.f, 1.f, 0.f, 1.0), Color(0.f, 0.5f, 0.f, 1.f));
	mAxisRenderer.SetAxisZColor(Color(0.f, 0.f, 1.f, 1.0), Color(0.f, 0.f, 0.5f, 1.f));
	mAxisRenderer.DrawAxis();
	mAxisRenderer.End();

	mGridRender.Begin(wvp3D);
	mGridRender.DrawGrid(Point(0,0,0));
	mGridRender.End();
	mTextRenderer.Begin(wvp);
	std::wstringstream str;
	str << std::fixed << std::setprecision(1) << L"FPS:" << debug->GetAverageFPS() <<"\n"<< L"FC:" << std::setprecision(4) <<debug->GetFrameCost() << "ms";
	mTextRenderer.DrawString(str.str().c_str(), 4, 4);
	
	RenderLayer(wvp);
	mTextRenderer.End();
	
}

void SecondScene::Updata(float deltaTime)
{
	UpdataLayer(deltaTime);
	mCamera.Updata();
	mCamera3D.Updata();
	auto ip = GetFramework()->GetInputManager();
	if (ip->IskeyDowm(DIK_ESCAPE))
		GetFramework()->Exit(0);
}

void SecondScene::OnSize(int ClientX, int ClientY)
{
	mCamera3D.UpdataProject(ClientX, ClientY);
	mCamera.UpdataProject(ClientX, ClientY);
}

void SecondScene::OnActivate(bool isActivate)
{
	if (isActivate)
	{
		mLabel2.SetText(L"Activate!");
	}
	else
	{
		mLabel2.SetText(L"No Activate!");
	}
}
void SecondScene::OnMouseMove(const MousePoint & mm, int pk)
{
	float h = GetFramework()->GetWindowsHeight();
	float w = GetFramework()->GetWindowsWidth();
	mCamera3D.Pitch(mCamera3D.GetFovAngle() / h *mm.y);
	mCamera3D.Yaw(mCamera3D.GetFovAngle() / w *mm.x);
}

void SecondScene::OnKeyDowm(Key k)
{
	switch (k)
	{
	case DIK_W:
		mCamera3D.Walk(1);
		break;
	case DIK_S:
		mCamera3D.Walk(-1);
		break;
	case DIK_A:
		mCamera3D.Strafe(-1);
		break;
	case DIK_D:
		mCamera3D.Strafe(1);
		break;
	case DIK_Z:
		mCamera3D.Fly(-1);
		break;
	case DIK_X:
		mCamera3D.Fly(1);
		break;
	case DIK_R:
		mCamera3D.Pitch(1);
		break;
	case DIK_Y:
		mCamera3D.Pitch(-1);
		break;
	case DIK_F:
		mCamera3D.Yaw(1);
		break;
	case DIK_H:
		mCamera3D.Yaw(-1);
		break;
	default:
		break;
	}
}