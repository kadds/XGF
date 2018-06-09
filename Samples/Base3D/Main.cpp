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
	
	virtual void OnCreate(GDI * gdi) override
	{
		mFont.Initialize(gdi, Tools::GetFontPath(L"msyh"), 16);
		mTextRenderer.Initialize(gdi, &mFont, 64);
		mMeshRenderer.Initialize(gdi);
		box = new Shape::BoxGeometry(0.3, 0.3, 0.3);
		material = new Shape::BasicMaterial(SM::Color(0, 1, 1, 1));
		mesh = new Shape::Mesh(box, material);

		mMeshRenderer.Add(mesh);
		mAxisRenderer.Initialize(gdi, 1e5f);
		mAxisRenderer.SetAxisXColor(SM::Color(1.f, 0.f, 0.f, 1.f), SM::Color(0.8f, 0.f, 0.f, 1.f));
		mAxisRenderer.SetAxisYColor(SM::Color(0.f, 1.f, 0.f, 1.f), SM::Color(0.f, 0.8f, 0.f, 1.f));
		mAxisRenderer.SetAxisZColor(SM::Color(0.f, 0.f, 1.f, 1.f), SM::Color(0.f, 0.f, 0.8f, 1.f));
		mCamera3D.LookAt(Point(-1.f, -1.f, 1.f), Point(0, 0, 0), Point::Up);
		mTrackballCameraController.SetAllSpeed(0.004);
		this->GetRootContainer().GetEventDispatcher().InsertAllEventListener( std::bind(&GameScene::OnMouse, this, std::placeholders::_1));
	};
	virtual void OnDestroy() override
	{
		mAxisRenderer.Shutdown();
		mMeshRenderer.Shutdown();
		mTextRenderer.Shutdown();
		mFont.Shutdown();
		delete box;
		delete material;
		delete mesh;
	};
	virtual void Render(float deltaTime) override
	{
		WVPMatrix wvp2d, wvp3d;
		this->Clear(SM::Color(0.7f, 0.7f, 0.7f, 1));
		mCamera2D.GetCameraMatrix(wvp2d);
		mCamera3D.GetCameraMatrix(wvp3d);

		mMeshRenderer.Begin(wvp3d);

		mMeshRenderer.Draw();
		mMeshRenderer.End();

		mAxisRenderer.Begin(wvp3d);
		mAxisRenderer.DrawAxis();
		mAxisRenderer.End();

		mTextRenderer.Begin(wvp2d);
		auto debug = DebugInscriber::GetInstance();
		std::wstringstream str;
		str.str(L"");
		str << std::fixed << std::setprecision(1) << L"FPS:" << debug->GetAverageFPS() << "\n" << L"FC:" << std::setprecision(4) << debug->GetFrameCost() << "ms";
		mTextRenderer.DrawString(str.str().c_str(), 4, 4);
		mTextRenderer.End();
	};
	virtual void Update(float deltaTime) override
	{
		if (this->GetFramework()->GetInputManager()->IskeyDowm(DIK_A))
		{
			mCamera3D.LookAt(Point(1.f, 1.f, 1.f), Point(0, 0, 0), Point::Up);
		}
		if (this->GetFramework()->GetInputManager()->IskeyDowm(DIK_S))
		{
			mCamera3D.LookTo(Point(1.f, 1.f, 1.f), Point(-1, 1, -1), Point::Up);
		}
		mCamera3D.Update();
		mCamera2D.Update();
	};
	virtual void OnSize(int ClientX, int ClientY) override
	{
		mCamera3D.UpdateProject(ClientX, ClientY);
		mCamera2D.UpdateProject(ClientX, ClientY);
	};

	void OnMouse(const Event & ev) 
	{
		mTrackballCameraController.HandleEvents(ev, mFramework->GetWindowsWidth(), mFramework->GetWindowsHeight());
	}

private:
	PerspectiveCamera mCamera3D;
	OrthoCamera mCamera2D;
	Font mFont;
	TextRenderer mTextRenderer;
	
	Shape::BoxGeometry * box;
	Shape::Material * material;
	Shape::Mesh * mesh;

	Shape::MeshRenderer mMeshRenderer;
	AxisRenderer mAxisRenderer;

	TrackballCameraController mTrackballCameraController = &mCamera3D;
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
	rt = app.CreateWindowsAndRunApplication(framework, gdi, hInstance, L"main", L"Base3D",
		0, 0, { 300, 100 }, { 600, 400 }, true, gs);
	return rt;
}