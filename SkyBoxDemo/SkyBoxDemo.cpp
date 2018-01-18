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
using namespace XGF;

class GameScene : public Scene
{
private:
	Font font;
	TextRenderer textRenderer;
	OrthoCamera camera2d;
	Batch batch;
	Batch textureBatch;
	Shape::Cube cube;
	BindingBridge bbg;
	Shaders shaders;

	VertexShader vss;
	PixelShader pss;
	AxisRenderer ar;
	Texture tt;
	PolygonPleTextureBinder cb;
	FPSCamera camera3d;
	ParticleFire ps;
	ParticleFire::FireEmitter eemitter;
	Texture particleT;
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
		GetFilePath(L"../../fx/fx/shaderCube.fx", wbuffer, MAX_PATH);
		vss.Initialize(gdi, wbuffer);
		pss.Initialize(gdi, wbuffer);
		shaders = { &vss, &pss, nullptr };
		batch.Initialize(gdi, shaders, 100, 100);
		batch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthEnableWithLessEqual);
		batch.GetShaderStage()->SetPSSamplerState(0, SamplerState::LineMirror);
		cube.SetPositionAndSize(-100, -100, -100, 200, 200, 200);
		cube.Filp();
		camera3d.SetPos(XMFLOAT3(100.0f, 1.f, 5.f));
		textureBatch.Initialize(gdi, ConstantData::GetInstance().GetPTShaders(), 20, 24);
		textureBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthDisable);
		textureBatch.GetShaderStage()->SetBlendState(BlendState::AddZeroOneAdd);
		ps.Initialize(gdi, 1024, ParticleDevice::Auto);
		ar.Initialize(gdi);
		
		tt.LoadDDS(gdi, GetFilePath(L"grasscube1024.dds", wbuffer, MAX_PATH));

		mFramework->GetInputManager()->GetCursor()->SetStaticTexture(mFramework->GetGDI(), GetFilePath(L"cursor.png", wbuffer, MAX_PATH));
		mFramework->GetInputManager()->SetMouseMode(MouseMode::CustomCenter);
		
		eemitter.pos = Point(200, 200, 0);
		eemitter.SetWidthAndHeight(15, 15);
		eemitter.startRadius = 20;
		eemitter.endRadius = 200;
		eemitter.frequency = 800;
		eemitter.mIgnoreZ = true;
		eemitter.SetColor(Color(1, 0, 0, 1), Color(1, 0, 0, 1), Color(1, 0, 0, 0.4), Color(1, 0, 0, 0.4));
		eemitter.SetEmitterAngle(0, 0, 360, 360);
		eemitter.accelerationMin = 10;
		eemitter.accelerationMax = 40;
		eemitter.velocityMin = 50;
		eemitter.velocityMax = 50;
		eemitter.SetGravity({ 0,1,0 }, 200);
		ps.AddEmitter(&eemitter);
		bbg.AddBinder(cube.mPolygon);
		GetFilePath(L"particle.png", wbuffer, MAX_PATH);
		particleT.LoadWIC(gdi, wbuffer);
		ps.SetTexture(&particleT);

		mFramework->GetEventDispatcher().InsertMouseEventListener(MouseEventId::MouseMove, std::bind(&GameScene::OnMouseMove, this, std::placeholders::_1));
	}
	virtual void OnDestroy() override
	{
		batch.Shutdown();
		ps.Shutdown();
		textRenderer.Shutdown();
		font.Shutdown();
		tt.Release();
		ar.Shutdown();
		vss.Shutdown();
		textureBatch.Shutdown();
		pss.Shutdown();
		particleT.Release();
	}
	virtual void Render(float deltaTime) override
	{
		auto gdi = mFramework->GetGDI();
		gdi->Clear(Color(0.4f, 0.4f, 0.4f, 1.0f));
		WVPMatrix wvp2d, wvp3d;
		camera2d.GetCameraMatrix(wvp2d);
		camera3d.GetCameraMatrix(wvp3d);
		
		batch.GetShaderStage()->SetVSConstantBuffer(0, &wvp3d);
		batch.GetShaderStage()->SetPSSRV(0, tt.GetShaderResourceView());
		batch.Begin();
		batch.DrawPolygon(cube.mPolygonPleIndex, bbg);
		
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
		ps.Begin(wvp3d);
		ps.Draw();
		ps.End();
		BindingBridge bbr;
		Shape::Rectangle rc;
		PolygonPleTextureBinder ttt(4);
		bbr.AddBinder(rc.mPolygon);
		bbr.AddBinder(ttt);
		ttt.SetPosition(0, 1, 0, 1);
		rc.SetPositionAndSize(100, 2, 50, 50);
		rc.SetZ(0);
		textureBatch.GetShaderStage()->SetVSConstantBuffer(0, &wvp3d);
		textureBatch.Begin();
		textureBatch.DrawPolygon( rc.mPolygonPleIndex,bbr);
		textureBatch.GetShaderStage()->SetPSSRV(0, particleT.GetShaderResourceView());
		textureBatch.End();

	}
	virtual void Updata(float deltaTime) override
	{
		camera2d.Updata();
		camera3d.Updata();
		cube.SetCenterPositionAndSize(camera3d.GetPosition(), XMFLOAT3( 5,5,5 ));
		batch.GetShaderStage()->SetVSConstantBuffer(batch.GetShaderStage()->GetConstantBufferIndexByName<VertexShader>("CBCameraPos"),&camera3d.GetPosition());
		auto ip = GetFramework()->GetInputManager();
		if (ip->IskeyDowm(DIK_ESCAPE))
			GetFramework()->Exit(0);
		float dt = deltaTime * 5;
		if (ip->IskeyDowm(DIK_LSHIFT))
		{
			dt *= 10;
		}
		if (ip->IskeyDowm(DIK_LCONTROL))
		{
			dt /= 10;
		}
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
		ps.Updata(deltaTime);
	}
	virtual void OnSize(int ClientX, int ClientY) override
	{
		camera2d.UpdataProject(ClientX, ClientY);
		camera3d.UpdataProject(ClientX, ClientY);
		
	}
	void OnMouseMove(const Event & ev)
	{
		float h = GetFramework()->GetWindowsHeight();
		float w = GetFramework()->GetWindowsWidth();
		camera3d.Pitch(camera3d.GetFovAngle() / h * ev.GetDataInt(1));
		camera3d.Yaw(camera3d.GetFovAngle() / w * ev.GetDataInt(0));
	}
};
int RunGame(HINSTANCE hInstance)
{
	Application app;
	GDI gdi;
	GameScene gs;//储存在栈中
	XGFramework framework;
	framework.SetSceneDeleter([](Scene * sc) {});//不需要显示delete
	framework.SetOnClose([]() {return true;});
	int rt = -1;
	rt = app.CreateWindowsAndRunApplication(framework, gdi, hInstance, L"sky", L"sky_xgf",
		0, 0, { 300,100 }, { 600,400 }, false, &gs);
	return rt;
}
