#define _XGF_DEBUG_ALLOC
#include "./../../XGF/Include/XGF.h"
#include "./../../XGF/Extra/imgui/imgui_impl.hpp"
#include <fmt/format.h>
using namespace XGF;
#include <iomanip>
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#define new  new(_CLIENT_BLOCK, __FILE__, __LINE__)  
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
		auto& context = Context::Current();
		auto& gdi = context.QueryGraphicsDeviceInterface();

		context.QueryFramework().GetInputManager().SwitchToDInput();
		imgui.Initialize();
		this->GetRootContainer().GetEventDispatcher().InsertAllEventListener(bind(&UI::Imgui::OnInputEvent, &imgui, placeholders::_1));

		context.QueryRenderer().SetLimitFrameRate(180);
		context.QueryFramework().SetLogicalFrameRate(15);
		context.QueryFramework().SetInfoFrameCost(1.f / 10);
		auto res = std::vector<ResourceInfo>();
		res.push_back(ResourceInfo(L"logo.png", L"logo"));

		mDirectionalLight = make_unique<Shape::DirectionalLight>(Point(5.f, -10.f, 5.f), Color(0.4f, 0.4f, 0.85f, 1.f));
		mPointLight = make_unique<Shape::PointLight>(Point(-3.f, 7.f, -4.f), Color(0.6f, 0.6f, 0.6f, 1.f), Point(0.1f, 0.001f, 0.0000001f));
		mSpotLight = make_unique<Shape::SpotLight>(Point(4.f, 5.f, 1.f), Point(-1.8f, -2.1f, -0.3f), Color(0.95f, 0.65f, 0.68f, 1.f), 
			Point(1.f, 0.f, 0.f), cos(DirectX::XM_PI / 4.f), cos(DirectX::XM_PI / 3.f));
		mAmbientLight = make_unique<Shape::AmbientLight>(Color(0.1f, 0.1f, 0.1f, 1.f));
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

		mDirectionalLight->SetShadowArea(Point(0, 5, 0), CameraRectangle(-20.f, 20.f, 20.f, -20.f, 0.1f, 20.f));
		auto setAction = [this] (Point & p)
		{
			std::vector<std::unique_ptr<Action>> actions;
			auto action1 = std::make_unique<DataAction<Point>>(Point(0, 0, -10), 3.0f, LinearInterpolator::GetInterpolator(), &p);
			actions.push_back(std::move(action1));

			auto action2 = std::make_unique<DataAction<Point>>(Point(-10, 0, 0), 3.0f, LinearInterpolator::GetInterpolator(), &p);
			actions.push_back(std::move(action2));

			auto action3 = std::make_unique<DataAction<Point>>(Point(0, 0, 10), 3.0f, LinearInterpolator::GetInterpolator(), &p);
			actions.push_back(std::move(action3));

			auto action4 = std::make_unique<DataAction<Point>>(Point(10, 0, 0), 3.0f, LinearInterpolator::GetInterpolator(), &p);
			actions.push_back(std::move(action4));

			auto comp = std::make_unique<SequenceAction>(std::move(actions));
			auto repeat = std::make_unique<RepeatAction>(std::move(comp), -1);
			mActions.AddAction(std::move(repeat));
		};
		setAction(mDirectionalLight->GetDirection());
		

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
				//make_unique<Shape::PhysicsMaterial>(randomColor(), Color3(0, 0, 0), 0.5f, 0.3f, 0.2f, 0.2f,  rand(random) > 0 ? texture : nullptr)
				);
			boxMesh[i]->GetMaterialAs<Shape::LightMaterial>()->SetLightGroup(1);
			boxMesh[i]->GetGeometry()->GetTransform().SetTranslation(Point(rand(random), 0, rand(random)));
			mMeshRenderer.Add(boxMesh[i].get());
			boxMesh[i]->SetCastShadow(true);
			boxMesh[i]->GetMaterialAs<Shape::LightMaterial>()->SetReceiveShadow(true);
		}
		
		
		planeMesh = make_unique<Shape::Mesh>(
			make_unique<Shape::PlaneGeometry>(100.f, 100.f,  1, 1),
			make_unique<Shape::PhongMaterial>(Color3(.8f, .8f, .8f), nullptr, Color3(0.8f, 0.8f, 0.82f))
		);
		planeMesh->GetMaterialAs<Shape::LightMaterial>()->SetLightGroup(1);
		planeMesh->GetGeometry()->GetTransform().TranslateToY(-0.51f);
		planeMesh->SetCastShadow(true);
		planeMesh->GetMaterialAs<Shape::LightMaterial>()->SetReceiveShadow(true);

		for(int i = 0; i < sizeof(sphereMesh) / sizeof(sphereMesh[0]); i++)
		{
			sphereMesh[i] = make_unique<Shape::Mesh>(
				make_unique<Shape::SphereGeometry>(1.f, 16, 16),
				make_unique<Shape::LambertMaterial>(randomColor(), rand(random) > 0 ? texture : nullptr, Color3(1, 1, 1))
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
		imgui.Shutdown();
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
		auto & context = Context::Current();
		
		context.QueryRenderer().PushDefaultFrameTarget();
		context.QueryRenderer().Clear(Color(0.7, 0.7, 0.7, 1));
		
		WVPMatrix wvp2d, wvp3d;
		mCamera2D.GetCameraMatrix(wvp2d);
		mCamera3D.GetCameraMatrix(wvp3d);
		
		mMeshRenderer.Begin(mCamera3D);

		mMeshRenderer.Draw();
		mMeshRenderer.End();

		mAxisRenderer.Begin(wvp3d);
		mAxisRenderer.DrawAxis();
		mAxisRenderer.End();

		mTextRenderer.Begin(wvp2d);
		auto debug = DebugInscriber::GetInstance();
		std::wstring s = fmt::format(L"FPS:{0}\nFC:{1}ms", debug->GetAverageFPS(), debug->GetFrameCost());
		mTextRenderer.DrawString(s, 4, 4);
		mTextRenderer.End();
		imgui.Begin();
		{
			ImGui::Begin("Info");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		imgui.End();

	};
	virtual void Update(float deltaTime) override
	{
		mCamera3D.Update();
		mCamera2D.Update();
		mActions.Update(deltaTime);
	};
	virtual void OnSize(int cx, int cy) override
	{
		mCamera3D.UpdateProject(cx, cy);
		mCamera2D.UpdateProject(cx, cy);
		mRc.SetPositionAndSize(0, 0, cx / 4.f, cy / 4.f);
		Context::Current().QueryRenderer().WaitFrame();
		imgui.Resize(cx, cy);
	};

	void OnEvent(const Event & ev) 
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		if(ev.GetEventGroup() == EventGroupType::Mouse)
			mTrackballCameraController.HandleEvents(ev, gdi.GetWidth(), gdi.GetHeight());
		else if(ev.GetEventGroup() == EventGroupType::KeyBoard && ev.GetKeyBoardEventId() == KeyBoardEventId::KeyDown)
		{
			if(ev.GetData<Input::KeyBoardKey>(0) == Input::KeyBoardKey::F2)
			{
				mDirectionalLight->SetCastShadow(!mDirectionalLight->GetCastShadow());
				mMeshRenderer.ReBuild();
			}
			if (ev.GetData<Input::KeyBoardKey>(0) == Input::KeyBoardKey::F3)
			{
				mSpotLight->SetCastShadow(!mSpotLight->GetCastShadow());
				mMeshRenderer.ReBuild();
			}
			if (ev.GetData<Input::KeyBoardKey>(0) == Input::KeyBoardKey::F4)
			{
				mDirectionalLight->SetGroup(!mDirectionalLight->GetGroup());
				mMeshRenderer.ReBuild();
			}
			if (ev.GetData<Input::KeyBoardKey>(0) == Input::KeyBoardKey::F5)
			{
				mSpotLight->SetGroup(!mSpotLight->GetGroup());
				mMeshRenderer.ReBuild();
			}
			if (ev.GetData<Input::KeyBoardKey>(0) == Input::KeyBoardKey::RightCommand)
			{
				mCamera3D.LookAt(Point(5.f, 5.f, 5.f), Point(0, 0, 0), Point::Up);
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

	Actions mActions;

	UI::Imgui imgui;
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