
#include "SecondScene.hpp"



SecondScene::SecondScene()
{
}


SecondScene::~SecondScene()
{
}

void SecondScene::OnCreate()
{
	char cbuffer[MAX_PATH + 1];
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

	mFont.Initialize(mFramework->GetGDI(), GetFilePath("msyh.ttf", cbuffer, MAX_PATH + 1), 16);
	mTextRenderer.Initialize(mFramework->GetGDI(), &mFont, 140);
	mUIBatches.Initialize(mFramework->GetGDI(), mBatches);
}
void SecondScene::OnDestory()
{
	mTextRenderer.Shutdown();
	mFont.Shutdown();
	mUIBatches.Shutdown(mBatches);
	GetFramework()->RemoveInputListener(&mUILayer);
}

void SecondScene::Render(float deltaTime)
{
	mFPS.Tick(deltaTime);
	WVPMatrix wvp;
	mCamera.GetCameraMatrix(wvp);


	mUIBatches.Begin();
	mTextRenderer.Begin(wvp);
	mFPS.RenderFPS(mTextRenderer, Color(1.f, 1.f, 0.4f, 1.f), 4, 4);
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
