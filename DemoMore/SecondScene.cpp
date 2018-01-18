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
	mLabel.SetPositionAndSize(20, 60, 100, 40);
	mLabel.SetText(L"第二个界面 按Esc退出");
	mLabel.SetZ(0.5);
	mLabel2.SetZ(0.4);
	//mCamera3D.FixYAxis(true);
	mLabel2.SetPositionAndSize(20, 100, 100, 40);
	mLabel2.SetText(L"Activate!");

	GetRootContainer().AddChild(mLabel2);
	GetRootContainer().AddChild(mLabel);

	Tools::GetInstance()->GetFontPath("Dengb.ttf", cbuffer, MAX_PATH);
	mFont.Initialize(mFramework->GetGDI(), cbuffer, 16);
	mTextRenderer.Initialize(mFramework->GetGDI(), &mFont, 140);
	mUITextRenderer.Initialize(mFramework->GetGDI(), &mFont, 140);
	mFramework->GetUIBatches().SetTextRenderer(BATCHES_TEXTRENDERER_DEFAULT_SIZE, &mUITextRenderer);

	mGridRender.Initialize(mFramework->GetGDI(), 10, 10, 5, 5);
	mAxisRenderer.Initialize(mFramework->GetGDI());

	mCursorTexture.LoadWIC(mFramework->GetGDI(), GetFilePath(L"tcursor.png", buffer, MAX_PATH));
	//mUILayer.GetUIBatches()->SetTextRenderer(BATCHES_TEXTRENDERER_DEFAULT_SIZE, &mUITextRenderer);
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

	mCamera3D.SetPos(XMFLOAT3(1, 1, 10));

	mFramework->GetEventDispatcher().InsertMouseEventListener(MouseEventId::MouseMove, std::bind(&SecondScene::OnMouseMove, this, std::placeholders::_1));
}
void SecondScene::OnDestroy()
{
	mFramework->GetEventDispatcher().RemoveMouseEventListener(MouseEventId::MouseMove, std::bind(&SecondScene::OnMouseMove, this, std::placeholders::_1));
	mTextRenderer.Shutdown();
	mFont.Shutdown();
	mCursorTexture.Release();
	mAxisRenderer.Shutdown();
	mUITextRenderer.Shutdown();
	mGridRender.Shutdown();
}

void SecondScene::Render(float deltaTime)
{
	Clear(Color(0.5f, 0.5f, 0.5f, 1.0f));
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
	
	mTextRenderer.End();
	
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
	mCamera3D.UpdataProject(ClientX, ClientY);
	mCamera.UpdataProject(ClientX, ClientY);
}

void SecondScene::OnActivate(bool isActivate)
{
	if(isActivate)
		mLabel2.SetText(L"Activate");
	else
		mLabel2.SetText(L"Un Activate");
}

void SecondScene::OnMouseMove(const Event & ev)
{
	float h = GetFramework()->GetWindowsHeight();
	float w = GetFramework()->GetWindowsWidth();
	mCamera3D.Pitch(mCamera3D.GetFovAngle() / h * ev.GetDataInt(1));
	mCamera3D.Yaw(mCamera3D.GetFovAngle() / w * ev.GetDataInt(0));
}

