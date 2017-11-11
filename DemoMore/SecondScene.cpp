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

	mLable2.SetPositionAndSize(20, 100, 100, 40);
	mLable2.SetTextRenderer(&mTextRenderer);
	mLable2.SetText(L"Activate!");
	mLable2.SetClickable(true);
	mLable2.SetMouseEventable(true);
	mLable2.AddOnClickListener([this](const MousePoint & ms, int pk)
	{
		mLable2.SetText(L"Click!");
		this->GetFramework()->GetGDI()->SetFullScreen(false);
	});
	mUILayer.Add(&mLable2);
	Tools::GetInstance()->GetFontPath("Dengb.ttf", cbuffer, MAX_PATH);
	mFont.Initialize(mFramework->GetGDI(), cbuffer, 16);
	mTextRenderer.Initialize(mFramework->GetGDI(), &mFont, 140);
	mUIBatches.Initialize(mFramework->GetGDI(), mBatches);
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
	GetFramework()->GetInputManager()->GetCursor()->SetAnimation(mAnimation);
	GetFramework()->GetInputManager()->GetCursor()->SetPointDeviation(Position(3.f, 0.f));
	GetFramework()->GetInputManager()->SetMouseMode(MouseMode::Custom);
}
void SecondScene::OnDestory()
{
	mTextRenderer.Shutdown();
	mFont.Shutdown();
	mUIBatches.Shutdown(mBatches);
	mCursorTexture.Release();
	GetFramework()->RemoveInputListener(&mUILayer);
}

void SecondScene::Render(float deltaTime)
{
	Clear(Color(0.5f, 0.5f, 0.5f, 0.0f));
	WVPMatrix wvp;
	mCamera.GetCameraMatrix(wvp);

	auto debug = DebugInscriber::GetInstance();
	mUIBatches.Begin();
	mTextRenderer.Begin(wvp);
	std::wstringstream str;
	str << std::fixed << std::setprecision(1) << L"FPS:" << debug->GetAverageFPS() <<"\n"<< L"FC:" << std::setprecision(4) <<debug->GetFrameCost() << "ms";
	mTextRenderer.DrawString(str.str().c_str(), 4, 4);
	//mTextRenderer.DrawString(str.str().c_str(), 4, 20);
	UpdataLayer(deltaTime);
	RenderLayer(mBatches);
	mTextRenderer.End();
	mUIBatches.End();
	
}

void SecondScene::Updata(float deltaTime)
{
	mCamera.Updata();
	auto ip = GetFramework()->GetInputManager();
	if (ip->IskeyDowm(DIK_ESCAPE))
		GetFramework()->Exit(0);
}

void SecondScene::OnSize(int ClientX, int ClientY)
{
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
