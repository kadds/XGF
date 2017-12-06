#include "GameScene.h"
#include <iomanip>


GameScene::GameScene()
{
}


GameScene::~GameScene()
{
}

void GameScene::MyRender(WVPMatrix & wvp, bool end)
{
	mFramework->ClearDepthStencilBuffer();
	mShapeRenderer.DrawCircle(240, 240, 40, GetPrecision(40, 5), 0.f, Color(1.f, 1.f, 0.f, 1.f), Color(0.f, 1.f, 0.f, 1.f));
	mTextRenderer.DrawString(L"Circle", 200, 180);
	mShapeRenderer.DrawRectangleB(50, 50, 100, 100, 0.1f, Color(1.f, 0.f, 0.0f, 1.f), 8.f, Color(0.f, 0.f, 1.f, 1.f), Color(0.f, 1.f, 0.f, 1.f));
	mTextRenderer.DrawString(L"RectangleB", 50, 100);
	mShapeRenderer.DrawRectangle(250, 50, 100, 100, 0.1f, Color(0.f, 0.2f, 0.7f, 1.f));
	mTextRenderer.DrawString(L"Rectangle", 280, 100);
	mShapeRenderer.DrawLine(160, 0, 160, 180, 0.f, Color(1.f, 1.f, 1.f, 1.f));
	mTextRenderer.DrawString(L"Line", 140, 188);
	mTextRenderer.DrawString(L"A RenderToTexture(RTT) Demo", 125, 35);
	if (!end)
	{
		mShapeRenderer.Flush();
		mTextRenderer.Flush();
	}
}

void GameScene::OnCreate()
{
	mShapeRenderer.Initialize(mFramework->GetGDI());
	char buffer[MAX_PATH];
	Tools::GetInstance()->GetFontPath("Dengb.ttf", buffer, MAX_PATH);
	mFont.Initialize(mFramework->GetGDI(), buffer, 16);
	mTextRenderer.Initialize(mFramework->GetGDI(), &mFont, 160);

	//ShaderLayout inputtype[2]{ &SHADER_EL_POSITION3,&SHADER_EL_TEXTURE };
	//mTextureShader.Initialize(mFramework->GetGDI(), ShaderConst::shaderPTVS, ShaderConst::shaderPTVSSize, ShaderConst::shaderPTPS, ShaderConst::shaderPTPSSize, inputtype, 2);
	//mTextureBatch.Initialize(mFramework->GetGDI(), &mTextureShader, 200, 400);
	//mRenderToTexture.Initialize(mFramework->GetGDI(), mFramework->GetGDI()->GetWidth(), mFramework->GetGDI()->GetHeight());

	mRc.SetPositionAndSize(0, mFramework->GetGDI()->GetHeight() - 100, 130, 100);
	mRc.SetZ(0.0f);
	mTime = 0.f;
}

void GameScene::OnDestory()
{
	//mRenderToTexture.Shutdown();
	//mTextureBatch.Shutdown();
	//mTextureShader.Shutdown();
	mTextRenderer.Shutdown();
	mFont.Shutdown();
	mShapeRenderer.Shutdown();
}

void GameScene::Render(float deltaTime)
{
	Clear(Color(0.5f, 0.5f, 0.5f, 0.0f));
	WVPMatrix wvp;
	mCamera.GetCameraMatrix(wvp);
	mShapeRenderer.Begin(wvp);
	mTextRenderer.Begin(wvp);
	//RTT start
	mRenderToTexture.SetRenderTarget();
	mRenderToTexture.Clear(Color(0.1f, 0.1f, 0.1f, 1.f));

	MyRender(wvp, false);

	//RTT end
	mRenderToTexture.SetDefaultRenderTarget();

	MyRender(wvp, true);

	mShapeRenderer.End();


	PolygonPleTextureBinder textureBinder(4);
	textureBinder.SetPosition(0.f, 1.f, 0.f, 1.f);
	BindingBridge bb;
	bb.AddBinder(textureBinder);

	std::wstringstream str;
	auto debug = DebugInscriber::GetInstance();
	str.clear();
	str.str(L"");
	
	str << std::fixed << std::setprecision(1) << L"FPS:" << debug->GetAverageFPS() << "\n" << L"FC:" << std::setprecision(4) << debug->GetFrameCost() << "ms";
	mTextRenderer.DrawString(str.str().c_str(), 4, 4);
	mTextRenderer.End();
	/*
	mTextureBatch.Begin(wvp);
	mRc.Render(mTextureBatch, nullptr, bb, mRenderToTexture.GetShaderResourceView());
	mTextureBatch.End();
	
	auto c = mRenderToTexture.GetShaderResourceView();
	ID3D11Resource *res;
	c->GetResource(&res);
	DirectX::SaveDDSTextureToFile(GetGDI()->GetDeviceContext(),res ,L"scene.dds");
	Exit(0);
	*/
}

void GameScene::Updata(float deltaTime)
{
	if (mFramework->GetInputManager()->IskeyDowm(DIK_F11) && mTime >= 5.f)
	{
		if (!mFramework->GetGDI()->IsFullScreen())
			mFramework->GetGDI()->SetDisplayMode(DisplayMode::FullScreen, 0, 0, 1440, 900, false, true);
		else
			mFramework->GetGDI()->SetDisplayMode(DisplayMode::Windowed, 400, 400, 600, 400, false, true);
		mTime = 0.f;
	}
	else if(mTime >= 0.f)
	{
		mTime += deltaTime;
		if (mTime > 50000.f) mTime = 1.f;
	}
		
}

void GameScene::OnSize(int ClientX, int ClientY)
{
	mCamera.UpdataProject(ClientX, ClientY);
	mRenderToTexture.Shutdown();
	mRenderToTexture.Initialize(mFramework->GetGDI(), mFramework->GetGDI()->GetWidth(), mFramework->GetGDI()->GetHeight());
	mRc.SetPositionAndSize(0, ClientY * 2 / 3, ClientX / 3, ClientY / 3);
}

void GameScene::OnActivate(bool isActivate)
{
}
