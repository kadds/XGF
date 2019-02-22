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
#include <random>
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
		auto & context = Context::Current();
		Context::Current().QueryRenderer().SetLimitFrameRate(180);
		Context::Current().QueryFramework().SetLogicalFrameRate(15);
		Context::Current().QueryFramework().SetInfoFrameCost(1.f / 10);
		auto res = std::vector<ResourceInfo>();
		res.push_back(ResourceInfo(L"logo.png", L"logo"));

		mDirectionalLight = make_unique<Shape::DirectionalLight>(Point(5, -5, 5), Color(0.4, 0.4, 0.85, 1));
		mPointLight = make_unique<Shape::PointLight>(Point(-3,7, -4), Color(0.6, 0.6, 0.6, 1), Point(0.1, 0.001, 0.0000001));
		mSpotLight = make_unique<Shape::SpotLight>(Point(4, 5, 1), Point(-1.8, -2.1, -0.3), Color(0.95, 0.65, 0.68, 1), 
			Point(1, 0, 0), cos(DirectX::XM_PI / 4), cos(DirectX::XM_PI / 3));
		mAmbientLight = make_unique<Shape::AmbientLight>(Color(0.1, 0.1, 0.1, 1));
		mDirectionalLight->SetGroup(1);
		mPointLight->SetGroup(1);
		mSpotLight->SetGroup(1);
		mAmbientLight->SetGroup(1);
		mSpotLight->SetShadowMapSize(1024, 1024);
		mSpotLight->SetShadowType(Shape::ShadowType::PCF);
		mSpotLight->SetCastShadow(true);
		mSpotLight->SetShadowArea(DirectX::XM_PI / 1.5f, 1.0f, 20.f);
		mDirectionalLight->SetShadowMapSize(1024, 1024);
		mDirectionalLight->SetShadowType(Shape::ShadowType::PCF);
		mDirectionalLight->SetCastShadow(true);
		mSpotLight->SetSlopeScaledDepthBias(1.5f);
		mDirectionalLight->SetSlopeScaledDepthBias(1.0f);

		mDirectionalLight->SetShadowArea(Point(-2, 4, -2), CameraRectangle(-15.f, 15.f, 15.f, -15.f, 1.f, 15.f));

		mTextureResourceManager.LoadResource(res);
		texture = mTextureResourceManager.GetResourceByAlias(L"logo");
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
			boxMesh[i]->GetGeometry()->GetTransform().SetTranslation(Point(rand(random), 0, rand(random)));
			mMeshRenderer.Add(boxMesh[i].get());
			boxMesh[i]->SetCastShadow(true);
			boxMesh[i]->GetMaterialAs<Shape::LightMaterial>()->SetReceiveShadow(true);
		}
		
		
		planeMesh = make_unique<Shape::Mesh>(
			make_unique<Shape::PlaneGeometry>(100, 100,  1, 1),
			make_unique<Shape::PhongMaterial>(Color3(.8f, .8f, .8f), nullptr, Color3(0.8, 0.8, 0.82))
		);
		planeMesh->GetMaterialAs<Shape::LightMaterial>()->SetLightGroup(1);
		planeMesh->GetGeometry()->GetTransform().TranslateToY(-0.51f);
		planeMesh->SetCastShadow(true);
		planeMesh->GetMaterialAs<Shape::LightMaterial>()->SetReceiveShadow(true);

		for(int i = 0; i < sizeof(sphereMesh) / sizeof(sphereMesh[0]); i++)
		{
			sphereMesh[i] = make_unique<Shape::Mesh>(
				make_unique<Shape::SphereGeometry>(1, 16, 16),
				make_unique<Shape::LambertMaterial>(randomColor(), rand(random) > 0 ? texture : nullptr,
					 Color3(1, 1, 1))
				);
			sphereMesh[i]->GetGeometry()->GetTransform().SetTranslation(Point(rand(random), 0.51f, rand(random)));

			sphereMesh[i]->GetMaterialAs<Shape::LightMaterial>()->SetLightGroup(1);
			sphereMesh[i]->SetCastShadow(true);
			mMeshRenderer.Add(sphereMesh[i].get());
			sphereMesh[i]->GetMaterialAs<Shape::LightMaterial>()->SetReceiveShadow(true);
		}
		
		mMeshRenderer.Add(planeMesh.get());


		baseMesh = make_unique<Shape::Mesh>(make_unique<Shape::BoxGeometry>(1, 1, 1, 2, 1, 3), make_unique<Shape::BasicMaterial>(Color(0.88, 0.89, 0)));
		baseMesh->GetGeometry()->GetTransform().SetTranslation(Point(2.f, 0, 1.f));
		baseMesh->SetCastShadow(true);

		mMeshRenderer.Add(baseMesh.get());

		mMeshRenderer.ReBuild();
		mAxisRenderer.Initialize(1e5f);
		mAxisRenderer.SetAxisXColor(Color(1.f, 0.f, 0.f, 1.f), Color(0.8f, 0.f, 0.f, 1.f));
		mAxisRenderer.SetAxisYColor(Color(0.f, 1.f, 0.f, 1.f), Color(0.f, 0.8f, 0.f, 1.f));
		mAxisRenderer.SetAxisZColor(Color(0.f, 0.f, 1.f, 1.f), Color(0.f, 0.f, 0.8f, 1.f));
		mCamera3D.SetMinDistance(0.1f);
		mCamera3D.SetMaxDistance(100.f);
		mCamera3D.LookAt(Point(3.f, 3.5f, 3.5f), Point(0, 0, 0), Point::Up);
		mTrackballCameraController.SetAllSpeed(0.004f);
		this->GetRootContainer().GetEventDispatcher().InsertAllEventListener(bind(&GameScene::OnEvent, this, placeholders::_1));

		mRc.SetZ(0.1f);
	};
	virtual void OnDestroy() override
	{
		mAxisRenderer.Shutdown();
		mMeshRenderer.Shutdown();
		mTextRenderer.Shutdown();
		mFont.Shutdown();
		planeMesh.reset(nullptr);
		for (int i = 0; i < sizeof(boxMesh) / sizeof(boxMesh[0]); i++)
		{
			boxMesh[i].reset();
		}
		for (int i = 0; i < sizeof(sphereMesh) / sizeof(sphereMesh[0]); i++)
		{
			sphereMesh[i].reset();
		}
		baseMesh.reset();
		mDirectionalLight.reset();
		mPointLight.reset();
		mSpotLight.reset();
		mAmbientLight.reset();
		mTextureResourceManager.ReleaseAllResource();
	};
	virtual void Render() override
	{
		WVPMatrix wvp2d, wvp3d;
		mCamera2D.GetCameraMatrix(wvp2d);
		mCamera3D.GetCameraMatrix(wvp3d);
		mMeshRenderer.Begin(mCamera3D);

		mMeshRenderer.Draw();
		mMeshRenderer.End();
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		Context::Current().QueryRenderer().Clear(Color(0.7, 0.7, 0.7, 1));
		PolygonPleTextureBinder textureBinder(4);
		textureBinder.GetData(1).x = textureBinder.GetData(0).x = 0.f;
		textureBinder.GetData(2).x = textureBinder.GetData(3).x = 1.f;
		textureBinder.GetData(1).y = textureBinder.GetData(2).y = 1.f;
		textureBinder.GetData(3).y = textureBinder.GetData(0).y = 0.f;

		BindingBridge bb;
		bb.AddBinder(mRc.mPolygon);
		bb.AddBinder(std::shared_ptr<PolygonPleTextureBinder>(&textureBinder, [](const PolygonPleTextureBinder *) {}));
		
		mAxisRenderer.Begin(wvp3d);
		mAxisRenderer.DrawAxis();
		mAxisRenderer.End();

		mTextRenderer.Begin(wvp2d);
		auto debug = DebugInscriber::GetInstance();
		std::wstring s = fmt::format(L"FPS:{0}\nFC:{1}ms", debug->GetAverageFPS(), debug->GetFrameCost());
		mTextRenderer.DrawString(s, 4, 4);
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
	virtual void OnSize(int cx, int cy) override
	{
		mCamera3D.UpdateProject(cx, cy);
		mCamera2D.UpdateProject(cx, cy);
		mRc.SetPositionAndSize(0, 0, cx / 4.f, cy / 4.f);

	};

	void OnEvent(const Event & ev) 
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		if(ev.GetEventGroup() == EventGroupType::Mouse)
			mTrackballCameraController.HandleEvents(ev, gdi.GetWidth(), gdi.GetHeight());
		else if(ev.GetEventGroup() == EventGroupType::KeyBoard && ev.GetKeyBoardEventId() == KeyBoardEventId::KeyDown)
		{
			if(ev.GetData<int>(0) == DIK_F2)
			{
				mDirectionalLight->SetCastShadow(!mDirectionalLight->GetCastShadow());
				mMeshRenderer.ReBuild();
			}
			if (ev.GetData<int>(0) == DIK_F3)
			{
				mSpotLight->SetCastShadow(!mSpotLight->GetCastShadow());
				mMeshRenderer.ReBuild();
			}
			if (ev.GetData<int>(0) == DIK_F4)
			{
				mDirectionalLight->SetGroup(!mDirectionalLight->GetGroup());
				mMeshRenderer.ReBuild();
			}
			if (ev.GetData<int>(0) == DIK_F5)
			{
				mSpotLight->SetGroup(!mSpotLight->GetGroup());
				mMeshRenderer.ReBuild();
			}
		}
	}

private:
	PerspectiveCamera mCamera3D;
	OrthoCamera mCamera2D;
	Font mFont;
	TextRenderer mTextRenderer;
	
	std::unique_ptr<Shape::Mesh> boxMesh[9], planeMesh, sphereMesh[4], baseMesh;

	Shape::MeshRenderer<Shape::ForwardRenderPath> mMeshRenderer;
	AxisRenderer mAxisRenderer;

	TrackballCameraController mTrackballCameraController = &mCamera3D;
	Texture * texture;
	TextureResourceManager mTextureResourceManager;

	std::unique_ptr<Shape::DirectionalLight> mDirectionalLight;
	std::unique_ptr<Shape::PointLight> mPointLight;
	std::unique_ptr<Shape::SpotLight> mSpotLight;
	std::unique_ptr<Shape::AmbientLight> mAmbientLight;

	Shape::Rectangle mRc;
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