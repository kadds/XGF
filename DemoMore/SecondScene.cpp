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
	mLable.SetPositionAndSize(20, 60, 100, 40);
	mLable.SetTextRenderer(&mTextRenderer);
	mLable.SetText(L"第二个界面 按Esc退出");
	mUILayer.Add(&mLable);
	mCamera3D.FixYAxis(true);
	mLable2.SetPositionAndSize(20, 100, 100, 40);
	mLable2.SetTextRenderer(&mTextRenderer);
	mLable2.SetText(L"Activate!");
	mLable2.SetClickable(true);
	mLable2.SetMouseEventable(true);
	mLable2.AddOnClickListener([this](const MousePoint & ms, int pk)
	{
		mLable2.SetText(L"Click!");
		//this->GetFramework()->GetGDI()->SetFullScreen(false);
	});
	mUILayer.Add(&mLable2);
	Tools::GetInstance()->GetFontPath("Dengb.ttf", cbuffer, MAX_PATH);
	mFont.Initialize(mFramework->GetGDI(), cbuffer, 16);
	mTextRenderer.Initialize(mFramework->GetGDI(), &mFont, 140);
	mUIBatches.Initialize(mFramework->GetGDI(), mBatches);
	mGridRender.Initialize(mFramework->GetGDI(), 10, 10, 5, 5);
	mAxisRenderer.Initialize(mFramework->GetGDI());

	mCursorTexture.LoadWIC(mFramework->GetGDI(), GetFilePath(L"tcursor.png", buffer, MAX_PATH));

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
}
void SecondScene::OnDestory()
{
	mTextRenderer.Shutdown();
	mFont.Shutdown();
	mUIBatches.Shutdown(mBatches);
	mCursorTexture.Release();
	mAxisRenderer.Shutdown();
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
	mUIBatches.Begin();
	mTextRenderer.Begin(wvp);
	std::wstringstream str;
	str << std::fixed << std::setprecision(1) << L"FPS:" << debug->GetAverageFPS() <<"\n"<< L"FC:" << std::setprecision(4) <<debug->GetFrameCost() << "ms";
	mTextRenderer.DrawString(str.str().c_str(), 4, 4);
	UpdataLayer(deltaTime);
	RenderLayer(mBatches);
	mTextRenderer.End();
	mUIBatches.End();
	
}

void SecondScene::Updata(float deltaTime)
{
	mCamera.Updata();
	mCamera3D.Updata();
	auto ip = GetFramework()->GetInputManager();
	if (ip->IskeyDowm(DIK_ESCAPE))
		GetFramework()->Exit(0);
}

void SecondScene::OnSize(int ClientX, int ClientY)
{
	mCamera3D.UpdataSize(ClientX, ClientY);
	mCamera.UpdataSize(ClientX, ClientY);
}

void SecondScene::OnActivate(bool isActivate)
{
	if (isActivate)
	{
		mLable2.SetText(L"Activate!");
	}
	else
	{
		mLable2.SetText(L"No Activate!");
	}
}
void SecondScene::OnMouseMove(const MousePoint & mm, int pk)
{
	float h = GetFramework()->GetWindowsHeight();
	float w = GetFramework()->GetWindowsWidth();
	mCamera3D.PitchYawRoll(-mCamera3D.GetFovAngle() / h  *mm.y * 0.5, mCamera3D.GetFovAngle() / w * mm.x * 0.5,0);
}

void SecondScene::OnKeyDowm(Key k)
{
	switch (k)
	{
	case DIK_W:
		mCamera3D.Walk(1);
		break;
	case DIK_S:
		mCamera3D.Walk(1);
		break;
	case DIK_A:
		mCamera3D.Strafe(1);
		break;
	case DIK_D:
		mCamera3D.Strafe(-1);
		break;
	case DIK_Z:
		mCamera3D.Fly(-1);
		break;
	case DIK_X:
		mCamera3D.Fly(1);
		break;
	case DIK_T:
		mCamera3D.Roll(1);
		break;
	case DIK_G:
		mCamera3D.Roll(-1);
		break;
	case DIK_R:
		mCamera3D.Pitch(0);
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