#define _XGF_DEBUG_ALLOC
#include "./../../XGF/Include/XGF.h"
using namespace XGF;
#include <iomanip>
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#define new  new(_CLIENT_BLOCK, __FILE__, __LINE__)  
#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib,"./../../Bin/x64/Debug/XGF.lib")
#else
#pragma comment(lib,"./../../Bin/Debug/XGF.lib")
#endif
#else
#ifdef _WIN64
#pragma comment(lib,"./../../Bin/x64/Release/XGF.lib")
#else
#pragma comment(lib,"./../../Bin/Release/XGF.lib")
#endif
#endif
int RunGame(HINSTANCE hInstance);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	int rt = RunGame(hInstance);
	return rt;
}
class GameScene :
	public Scene
{
public:
	GameScene() {};
	virtual ~GameScene() {};
	void MyRender(WVPMatrix & wvp, bool end)
	{
		mFramework->ClearDepthStencilBuffer();
		mShapeRenderer.DrawCircle(240, 240, 40, GetPrecision(40, 5), 0.01f, Color(1.f, 1.f, 0.f, 1.f), Color(0.f, 1.f, 0.f, 1.f));
		mTextRenderer.DrawString(L"Circle", 200, 180);
		mShapeRenderer.DrawRectangleC(50, 50, 100, 100, 0.1f, Color(1.f, 0.f, 0.0f, 1.f), 8.f, Color(0.f, 0.f, 1.f, 1.f), Color(0.f, 1.f, 0.f, 1.f));
		mTextRenderer.DrawString(L"RectangleB", 50, 100);
		mShapeRenderer.DrawRectangle(250, 50, 100, 100, 0.1f, Color(0.f, 0.2f, 0.7f, 1.f));
		mTextRenderer.DrawString(L"Rectangle", 280, 100);
		mShapeRenderer.DrawLine(160, 0, 160, 180, 0.2f, Color(1.f, 1.f, 1.f, 1.f));
		mTextRenderer.DrawString(L"Line", 140, 188);
		mShapeRenderer.DrawTriangle(Position(40, 200), Position(70, 210), Position(40, 240), 0.1f, Color(0.4f, 0.8f, 0.1f, 1.f));
		mTextRenderer.DrawString(L"Triangle", 40, 220);
		Position p[5] = { { 0,50 },{ 50,50 },{ 40,100 },{ 100,100 },{ 100,80 } };
		mShapeRenderer.DrawLineList(*p, 5, 0.001f, Color(0.0, 0.0, 0.0, 1.0));
		mTextRenderer.DrawString(L"LineList", 4, 70);
		Position p2[5] = { { 0,150 },{ 50,120 },{ 40,200 },{ 100,200 },{ 100,220 } };
		mShapeRenderer.DrawPolygonList(*p2, 5, 0.1f, Color(0.0f, 0.1f, 0.6f, 1.0f));
		mTextRenderer.DrawString(L"PolygonList", 40, 172);
		mTextRenderer.DrawString(L"A RenderToTexture(RTT) Demo", 125, 35);
		if (!end)
		{
			mShapeRenderer.Flush();
			mTextRenderer.Flush();
		}
	};
	virtual void OnCreate(GDI * gdi) override
	{
		mShapeRenderer.Initialize(gdi);
		char buffer[MAX_PATH];
		Tools::GetFontPath("Dengb.ttf", buffer, MAX_PATH);
		mFont.Initialize(gdi, buffer, 16);
		mTextRenderer.Initialize(gdi, &mFont, 240);

		mTextureBatch.Initialize(gdi, ConstantData::GetInstance().GetPTShaders(), 200, 400);
		mTextureBatch.GetShaderStage()->SetBlendState(BlendState::AddZeroOneAdd);
		mTextureBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthDisable);
		mRenderToTexture.Initialize(gdi, gdi->GetWidth(), gdi->GetHeight());

		mRc.SetPositionAndSize(0, gdi->GetHeight() - 100.f, 130.f, 100.f);
		mRc.SetZ(0.001f);
		mTime = 0.f;
	};
	virtual void OnDestroy() override
	{
		mRenderToTexture.Shutdown();
		mTextureBatch.Shutdown();
		mTextRenderer.Shutdown();
		mFont.Shutdown();
		mShapeRenderer.Shutdown();
	};
	virtual void Render(float deltaTime) override
	{
		Clear(Color(0.5f, 0.5f, 0.5f, 1.0f));
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
		mTextureBatch.GetShaderStage()->SetVSConstantBuffer(0, &wvp);
		mTextureBatch.Begin();
		mRc.Render(mTextureBatch, bb, mRenderToTexture.GetShaderResourceView());
		mTextureBatch.End();
		/*
		auto c = mRenderToTexture.GetShaderResourceView();
		ID3D11Resource *res;
		c->GetResource(&res);
		DirectX::SaveDDSTextureToFile(GetGDI()->GetDeviceContext(),res ,L"scene.dds");
		Exit(0);
		*/
	};
	virtual void Update(float deltaTime) override
	{
		if (mFramework->GetInputManager()->IskeyDowm(DIK_F11) && mTime >= 5.f)
		{
			auto gdi = mFramework->GetGDI();
			if (!gdi->IsFullScreen())
				gdi->SetDisplayMode(DisplayMode::FullScreen, 0, 0, 1440, 900, false, true);
			else
				gdi->SetDisplayMode(DisplayMode::Windowed, 400, 400, 600, 400, false, true);
			mTime = 0.f;
		}
		else if (mTime >= 0.f)
		{
			mTime += deltaTime;
			if (mTime > 50000.f) mTime = 1.f;
		}
	};
	virtual void OnSize(int ClientX, int ClientY) override
	{
		auto gdi = mFramework->GetGDI();
		mCamera.UpdateProject(ClientX, ClientY);
		mRenderToTexture.Shutdown();
		mRenderToTexture.Initialize(gdi, gdi->GetWidth(), gdi->GetHeight());
		mRc.SetPositionAndSize(0.f, ClientY * 2 / 3.f, ClientX / 3.f, ClientY / 3.f);
	};

private:
	ShapeRenderer mShapeRenderer;
	OrthoCamera mCamera;
	RenderToTexture mRenderToTexture;

	Font mFont;
	TextRenderer mTextRenderer;

	Batch mTextureBatch;

	Shape::Rectangle mRc;

	Texture mTexture;

	float mTime;
};


int RunGame(HINSTANCE hInstance)
{
	Application app;
	GDI gdi;
	GameScene * gs = new GameScene();
	XGFramework framework;
	framework.SetSceneDeleter([](Scene * sc) {delete sc; });
	framework.SetOnClose([]() {return true; });
	int rt = -1;
	rt = app.CreateWindowsAndRunApplication(framework, gdi, hInstance, L"tet", L"dsgdfhv,",
		0, 0, { 300, 100 }, { 600, 400 }, false, gs);
	return rt;
}