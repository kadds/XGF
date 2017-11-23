// SkyBoxDemo.cpp : 定义应用程序的入口点。
//
#include <Windows.h>
#define _XGF_DEBUG_ALLOC
#include "../XGF/Include/XGF.h"
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#define new  new(_CLIENT_BLOCK, __FILE__, __LINE__)  
#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib,"../Bin/x64/Debug/XGF.lib")
#else
#pragma comment(lib,"../Bin/Debug/XGF.lib")
#endif
#else
#ifdef _WIN64
#pragma comment(lib,"../Bin/x64/Release/XGF.lib")
#else
#pragma comment(lib,"../Bin/Release/XGF.lib")
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


class GameScene : public Scene, public InputListener
{
private:
	Font font;
	TextRenderer textRenderer;
	OrthoCamera camera2d;
	Batch batch;
	Shape::Cube cube;
	BindingBridge bbg;
	Shader shader;
	AxisRenderer ar;
	Texture tt;
	PolygonPleTextureBinder cb;
	FPSCamera camera3d;
	PointColorParticle ppcp;
	//ParticleSystem ps;
public:
	GameScene() :cb(cube.mPolygon.mCount){};
	virtual ~GameScene() {};
	virtual void OnCreate() override
	{
		char buffer[MAX_PATH];
		auto gdi = mFramework->GetGDI();
		wchar_t wbuffer[MAX_PATH];
		Tools::GetInstance()->GetFontPath("Dengb.ttf", buffer, MAX_PATH);
		font.Initialize(gdi, buffer, 14);
		textRenderer.Initialize(gdi, &font, 100);
		InputType it = SHADER_INPUTLAYOUT_POSITION;
		GetFilePath(L"../../fx/fx/shaderCube.fx", wbuffer, MAX_PATH);
		shader.Initialize(gdi, wbuffer, nullptr,&it,1);
		batch.Initialize(gdi,&shader, 100, 100);
		cube.SetPositionAndSize(-100, -100, -100, 200, 200, 200);
		cube.Filp();
		camera3d.SetPos(XMFLOAT3(20.0f, 1.f, 5.f));
		
		//ps.Initialize(gdi, 1000);
		ar.Initialize(gdi);
		
		tt.LoadDDS(gdi, GetFilePath(L"grasscube1024.dds", wbuffer, MAX_PATH));

		mFramework->GetInputManager()->GetCursor()->SetStaticTexture(mFramework->GetGDI(), GetFilePath(L"cursor.png", wbuffer, MAX_PATH));
		mFramework->GetInputManager()->SetMouseMode(MouseMode::CustomCenter);
		mFramework->AddInputListener(this);

		ppcp.SetPosition(Point(20, 0, 5));
	}
	virtual void OnDestory() override
	{
		Scene::OnDestory();
		mFramework->RemoveInputListener(this);
		batch.Shutdown();
		shader.Shutdown();
		//ps.Shutdown();
		textRenderer.Shutdown();
		font.Shutdown();
		tt.Release();
		ar.Shutdown();
	}
	virtual void Render(float deltaTime) override
	{
		auto gdi = mFramework->GetGDI();
		gdi->Clear(Color(0.4f, 0.4f, 0.4f, 1.0f));
		WVPMatrix wvp2d, wvp3d;
		camera2d.GetCameraMatrix(wvp2d);
		camera3d.GetCameraMatrix(wvp3d);

		batch.Begin(wvp3d);
		batch.SetTexture(tt);
		batch.DrawPolygon(cube.mPolygon, cube.mPolygonPleIndex, bbg);
		
		batch.End();
		ar.Begin(wvp3d);
		ar.SetAxisXColor(Color(1.f, 0.f, 0.f, 1.f), Color(0.8f, 0.f, 0.f, 1.f));
		ar.SetAxisYColor(Color(0.f, 1.f, 0.f, 1.f), Color(0.0f, 0.8f, 0.f, 1.f));
		ar.SetAxisZColor(Color(0.f, 0.f, 1.f, 1.f), Color(0.0f, 0.f, 0.8f, 1.f));
		ar.DrawAxis();
		ar.End();
		gdi->ClearDepthStencilBuffer();
		textRenderer.Begin(wvp2d);
		textRenderer.DrawStringEx(4, 4, Color(0.8f, 0.8f, 0.9f, 1.0f), L"FPS:%d", (int)DebugInscriber::GetInstance()->GetAverageFPS());
		textRenderer.End();
		//ps.Begin(wvp3d);
		//ps.Draw(ppcp);
		//ps.End();

	}
	virtual void Updata(float deltaTime) override
	{
		ppcp.Update();
		camera2d.Updata();
		camera3d.Updata();
		auto ip = GetFramework()->GetInputManager();
		if (ip->IskeyDowm(DIK_ESCAPE))
			GetFramework()->Exit(0);
		float dt = deltaTime * 5;
		if(ip->IskeyDowm(DIK_W))
		{
			camera3d.Walk(dt);
		}
		if (ip->IskeyDowm(DIK_S))
		{
			camera3d.Walk(-dt);
		}
		if (ip->IskeyDowm(DIK_A))
		{
			camera3d.Strafe(-dt);
		}
		if (ip->IskeyDowm(DIK_D))
		{
			camera3d.Strafe(dt);
		}
		if (ip->IskeyDowm(DIK_Z))
		{
			camera3d.Fly(-dt);
		}
		if (ip->IskeyDowm(DIK_X))
		{
			camera3d.Fly(dt);
		}
		
	}
	virtual void OnSize(int ClientX, int ClientY) override
	{
		camera2d.UpdataProject(ClientX, ClientY);
		camera3d.UpdataProject(ClientX, ClientY);
		
	}
	virtual void OnActivate(bool isActivate) override 
	{

	}
	virtual void OnMouseDowm(const MousePoint &mp, int pk)  override {};
	virtual void OnMouseUp(const MousePoint &mp, int pk) override {};
	virtual void OnMouseMove(const MousePoint &mm, int pk) override 
	{
		float h = GetFramework()->GetWindowsHeight();
		float w = GetFramework()->GetWindowsWidth();
		camera3d.Pitch(camera3d.GetFovAngle() / h *mm.y);
		camera3d.Yaw(camera3d.GetFovAngle() / w *mm.x);
	}
	virtual void OnKeyDowm(Key k) override 
	{
		
	}
	virtual void OnKeyUp(Key k)  override {};
};
int RunGame(HINSTANCE hInstance)
{
	Application app;
	GDI gdi;
	GameScene * gs = new GameScene();
	XGFramework framework;
	framework.SetSceneDeleter([](Scene * sc) {delete sc;});
	framework.SetOnClose([]() {return true;});
	int rt = -1;
	rt = app.CreateWindowsAndRunApplication(framework, gdi, hInstance, L"sky", L"sky_xgf",
		0, 0, { 300,100 }, { 600,400 }, false, gs);
	return rt;
}
