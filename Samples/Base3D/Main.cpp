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
	
	virtual void OnCreate() override
	{
		auto res = std::vector<ResourceInfo>();
		res.push_back(ResourceInfo(L"logo.png", L"logo"));

		mDirectionalLight = make_unique<Shape::DirectionalLight>(Point(-5, -5, -5), Color(0.3, 0.4, 0.4, 1));
		mPointLight = make_unique<Shape::PointLight>(Point(-3,7, -4), Color(0.6, 0.6, 0.6, 1), Point(0.1, 0.001, 0.0000001));
		mSpotLight = make_unique<Shape::SpotLight>(Point(5, 6, 5), Point(-1.4, -1.8, -1.2), Color(0.69, 0.65, 0.68, 1), 
			Point(1, 0, 0), cos(DirectX::XM_PI / 4), cos(DirectX::XM_PI / 3));
		mAmbientLight = make_unique<Shape::AmbientLight>(Color(0.1, 0.1, 0.1, 1));
		mDirectionalLight->SetGroup(1);
		mPointLight->SetGroup(1);
		mSpotLight->SetGroup(1);
		mAmbientLight->SetGroup(1);

		mTextureResourceManager.LoadResource(res);
		texture = new Texture(*mTextureResourceManager.GetResourceByAlias(L"logo"));

		mFont.Initialize(Tools::GetFontPath(L"msyh"), 16);
		mTextRenderer.Initialize(&mFont, 64);
		mMeshRenderer.Initialize();
		mMeshRenderer.Add(mDirectionalLight.get());
		mMeshRenderer.Add(mPointLight.get());
		mMeshRenderer.Add(mSpotLight.get());
		mMeshRenderer.Add(mAmbientLight.get());
		std::mt19937 random(1);
		std::uniform_real_distribution<float> rand(-7, 7);
		auto randomColor = [&random]() -> Color3
		{
			static Color3 c[] = {
				Color3(1, 0, 0),
				Color3(0, 1, 0),
				Color3(0, 0, 1),
				Color3(1, 1, 0),
				Color3(0, 1, 1),
				Color3(1, 0, 1),
			};
			std::uniform_int_distribution<int>rand(0, sizeof(c) / sizeof(c[0]) - 1);
			Color3 d = c[rand(random)];
			return d;
		};
		for(int i = 0; i < sizeof(boxMesh) / sizeof(boxMesh[0]); i++)
		{
			boxMesh[i] = make_unique<Shape::Mesh>(
				make_unique<Shape::BoxGeometry>(1.f, 1.f, 1.f),
				make_unique<Shape::PhongMaterial>(randomColor(), rand(random) > 0 ? texture : nullptr, Color3(1, 1, 1))
				);
			boxMesh[i]->GetMaterialAs<Shape::LightMaterial>()->SetLightGroup(1);
			boxMesh[i]->GetGeometry()->GetTransform().SetTranslation(Point(rand(random), 0.51f, rand(random)));
			mMeshRenderer.Add(boxMesh[i].get());
		}
		
		
		planeMesh = make_unique<Shape::Mesh>(
			make_unique<Shape::PlaneGeometry>(100, 100, 2, 2),
			make_unique<Shape::PhongMaterial>(Color3(.8f, .8f, .8f), nullptr, Color3(0.8, 0.8, 0.82))
		);
		planeMesh->GetMaterialAs<Shape::LightMaterial>()->SetLightGroup(1);
		planeMesh->GetGeometry()->GetTransform().TranslateToY(-2.01f);

		for(int i = 0; i < sizeof(sphereMesh) / sizeof(sphereMesh[0]); i++)
		{
			sphereMesh[i] = make_unique<Shape::Mesh>(
				make_unique<Shape::SphereGeometry>(1, 16, 16),
				make_unique<Shape::LambertMaterial>(randomColor(), rand(random) > 0 ? texture : nullptr,
					 Color3(1, 1, 1))
				);
			sphereMesh[i]->GetGeometry()->GetTransform().SetTranslation(Point(rand(random), 0.51f, rand(random)));

			sphereMesh[i]->GetMaterialAs<Shape::LightMaterial>()->SetLightGroup(1);
			mMeshRenderer.Add(sphereMesh[i].get());
		}
		
		planeMesh->GetMaterial()->SetRasterizerState(RasterizerState::SolidAndCutNone);
		
		mMeshRenderer.Add(planeMesh.get());
		mMeshRenderer.RefreshLightMesh();
		mAxisRenderer.Initialize(1e5f);
		mAxisRenderer.SetAxisXColor(Color(1.f, 0.f, 0.f, 1.f), Color(0.8f, 0.f, 0.f, 1.f));
		mAxisRenderer.SetAxisYColor(Color(0.f, 1.f, 0.f, 1.f), Color(0.f, 0.8f, 0.f, 1.f));
		mAxisRenderer.SetAxisZColor(Color(0.f, 0.f, 1.f, 1.f), Color(0.f, 0.f, 0.8f, 1.f));
		mCamera3D.SetMinDistance(0.1f);
		mCamera3D.SetMaxDistance(100.f);
		mCamera3D.LookAt(Point(3.f, 3.5f, 3.5f), Point(0, 0, 0), Point::Up);
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
		planeMesh.reset(nullptr);
		for (int i = 0; i < sizeof(boxMesh) / sizeof(boxMesh[0]); i++)
		{
			boxMesh[i].reset();
		}
		for (int i = 0; i < sizeof(sphereMesh) / sizeof(sphereMesh[0]); i++)
		{
			sphereMesh[i].reset();
		}
		mDirectionalLight.reset();
		mPointLight.reset();
		mTextureResourceManager.ReleaseAllResource();
	};
	virtual void Render(float deltaTime) override
	{
		WVPMatrix wvp2d, wvp3d;
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		gdi.Clear(Color(0.7f, 0.7f, 0.7f, 1));
		mCamera2D.GetCameraMatrix(wvp2d);
		mCamera3D.GetCameraMatrix(wvp3d);

		mMeshRenderer.Begin(wvp3d);

		mMeshRenderer.Draw(mCamera3D.GetPosition());
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
			mCamera3D.LookAt(Point(5.f, 5.f, 5.f), Point(0, 0, 0), Point::Up);
		}
		if (GetFramework().GetInputManager().IskeyDown(DIK_S))
		{
			mCamera3D.LookTo(Point(5.f, 5.f, 5.f), Point(-1, 1, -1), Point::Up);
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
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

		mTrackballCameraController.HandleEvents(ev, gdi.GetWidth(), gdi.GetHeight());
	}

private:
	PerspectiveCamera mCamera3D;
	OrthoCamera mCamera2D;
	Font mFont;
	TextRenderer mTextRenderer;
	
	std::unique_ptr<Shape::Mesh> boxMesh[9], planeMesh, sphereMesh[4];

	Shape::MeshRenderer mMeshRenderer;
	AxisRenderer mAxisRenderer;

	TrackballCameraController mTrackballCameraController = &mCamera3D;
	Texture * texture;
	TextureResourceManager mTextureResourceManager;

	std::unique_ptr<Shape::DirectionalLight> mDirectionalLight;
	std::unique_ptr<Shape::PointLight> mPointLight;
	std::unique_ptr<Shape::SpotLight> mSpotLight;
	std::unique_ptr<Shape::AmbientLight> mAmbientLight;
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