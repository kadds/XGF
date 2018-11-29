#define _XGF_DEBUG_ALLOC
#include "./../../XGF/Include/XGF.h"
#include <fmt/format.h>
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
using namespace std;

int RunGame(HINSTANCE hInstance);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return RunGame(hInstance);
}
class GameScene :
	public Scene
{
public:
	GameScene() {};
	virtual ~GameScene() {};
	
	virtual void OnCreate(GDI * gdi) override
	{
		auto res = std::vector<ResourceInfo>();
		res.push_back(ResourceInfo(L"logo.png", L"logo"));
		
		mTextureResourceManager.LoadResource(gdi, res);
			
		texture = new Texture(*mTextureResourceManager.GetResourceByAlias(L"logo"));

		mFont.Initialize(gdi, Tools::GetFontPath(L"msyh"), 16);
		mTextRenderer.Initialize(gdi, &mFont, 64);
		mMeshRenderer.Initialize(gdi);
		boxMesh = make_unique<Shape::Mesh>(
			make_unique<Shape::BoxGeometry>(1.f, 1.f, 1.f), 
			make_unique<Shape::BasicMaterial>(SM::Color(1.0f, 0.2f, 0.f, 1.f), texture)
		);
		boxMesh->GetGeometry()->GetTransform().TranslateToY(0.51f);
		
		planeMesh = make_unique<Shape::Mesh>(
			make_unique<Shape::PlaneGeometry>(10, 10, 2, 2),
			make_unique<Shape::BasicMaterial>(SM::Color(1.f, 1.f, 1.f, 1.f), texture)
		);
		planeMesh->GetGeometry()->GetTransform().TranslateToY(-0.01f);

		sphereMesh = make_unique<Shape::Mesh>(
			make_unique<Shape::SphereGeometry>(1, 16, 16),
			make_unique<Shape::BasicMaterial>(SM::Color(1.f, 1.f, 1.f, 1.f), texture)
		);
		sphereMesh->GetGeometry()->GetTransform().SetTranslation(Point(1, 1.1f, 1));
		//sphereMesh->GetMaterial()->SetRasterizerState(RasterizerState::SolidAndCutNone);
		mMeshRenderer.Add(boxMesh.get());
		mMeshRenderer.Add(planeMesh.get());
		mMeshRenderer.Add(sphereMesh.get());

		mAxisRenderer.Initialize(gdi, 1e5f);
		mAxisRenderer.SetAxisXColor(SM::Color(1.f, 0.f, 0.f, 1.f), SM::Color(0.8f, 0.f, 0.f, 1.f));
		mAxisRenderer.SetAxisYColor(SM::Color(0.f, 1.f, 0.f, 1.f), SM::Color(0.f, 0.8f, 0.f, 1.f));
		mAxisRenderer.SetAxisZColor(SM::Color(0.f, 0.f, 1.f, 1.f), SM::Color(0.f, 0.f, 0.8f, 1.f));
		mCamera3D.SetMinDistance(0.1f);
		mCamera3D.SetMaxDistance(100.f);
		mCamera3D.LookAt(Point(-1.5f, -1.5f, 1.5f), Point(0, 0, 0), Point::Up);
		mTrackballCameraController.SetAllSpeed(0.004f);
		this->GetRootContainer().GetEventDispatcher().InsertAllEventListener(bind(&GameScene::OnMouse, this, placeholders::_1));
	};
	virtual void OnDestroy() override
	{
		mAxisRenderer.Shutdown();
		mMeshRenderer.Shutdown();
		mTextRenderer.Shutdown();
		mFont.Shutdown();
		delete texture;
		boxMesh.reset(nullptr);
		planeMesh.reset(nullptr);
		sphereMesh.reset(nullptr);
		mTextureResourceManager.ReleaseAllResource();
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
		std::wstring s = fmt::format(L"FPS:{0}\nFC:{1}ms", debug->GetAverageFPS(), debug->GetFrameCost());
		mTextRenderer.DrawString(s.c_str(), 4, 4);
		mTextRenderer.End();
		
	};
	virtual void Update(float deltaTime) override
	{
		if (GetFramework().GetInputManager().IskeyDown(DIK_A))
		{
			mCamera3D.LookAt(Point(1.f, 1.f, 1.f), Point(0, 0, 0), Point::Up);
		}
		if (GetFramework().GetInputManager().IskeyDown(DIK_S))
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
		auto & framework = GetFramework();
		mTrackballCameraController.HandleEvents(ev, framework.GetWindowsWidth(), framework.GetWindowsHeight());
	}

private:
	PerspectiveCamera mCamera3D;
	OrthoCamera mCamera2D;
	Font mFont;
	TextRenderer mTextRenderer;
	
	std::unique_ptr<Shape::Mesh> boxMesh, planeMesh, sphereMesh;

	Shape::MeshRenderer mMeshRenderer;
	AxisRenderer mAxisRenderer;

	TrackballCameraController mTrackballCameraController = &mCamera3D;
	Texture * texture;
	TextureResourceManager mTextureResourceManager;
};


int RunGame(HINSTANCE hInstance)
{
	Application app;
	XGFramework framework;
	GDI gdi;

	auto gameScene = make_shared<GameScene>();
	framework.SetOnCloseListener(XGFramework::AutoClose());

	WindowProperty windowProperty;
	windowProperty.title = L"Base3D";
	windowProperty.className = L"Base3D";
	windowProperty.ICON = 0;
	windowProperty.SICON = 0;
	windowProperty.canResize = true;
	windowProperty.point = { 300, 100 };
	windowProperty.size = { 600, 400 };

	return app.CreateWindowsAndRunApplication(framework, gdi, hInstance, windowProperty, gameScene);
}