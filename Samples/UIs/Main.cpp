#define _XGF_DEBUG_ALLOC
#include "../../XGF/Include/XGF.h"
#include <fmt/format.h>
#include <memory>
using namespace std;
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
		mBtnGroup = make_unique<Container>();
		auto fname = Tools::GetFontPath(L"msyh");
		mFont.Initialize(fname, 16);
		mFont_s.Initialize(fname, 12);
		mFont_b.Initialize(fname, 20);
		mUITextRenderer.Initialize(&mFont, 1000);
		mTextRenderer_s.Initialize(&mFont_s, 1000);
		mTextRenderer_b.Initialize(&mFont_b, 200);
		auto & framework = GetFramework();
		framework.GetUIBatches().SetTextRenderer(FontSize::Default, &mUITextRenderer);
		GetRootContainer().AddChild(mBtnGroup);
		std::shared_ptr<Label> label = std::make_shared<Label>(0, L"Direct3D11");
		label->SetPositionAndSize(200, 200, 100, 20);
		label->SetZ(0.4f);
		GetRootContainer().AddChild(label);
		
		label->GetClickHelper().AddOnClickListener([this](Control *label, const MousePoint & ms, int mouseButton) {
			AsyncTask::NewTask(&GetFramework().GetThread(), [](std::shared_ptr<AsyncTask> asyn) {
				MessageBox(NULL, L"YOU CLICK Label!!", L"Exe", 0);
				asyn->Finish(0, 0);
			});
			if(label->GetShape()->GetTransform().GetTranslateX() < 180)
				label->GetShape()->GetTransform().AddTranslationAction(Action::Make(Point(200, 2, 0), 1.f, false, LinearInterpolator::GetInterpolator()));
			else
				label->GetShape()->GetTransform().AddTranslationAction(Action::Make(Point(50, 2, 0), 1.f, false, LinearInterpolator::GetInterpolator()));
		});
		
		std::shared_ptr<Button> nextButton = std::make_shared<Button>(2, L"switch to next scene.");
		mBtnGroup->AddChild(nextButton);
		nextButton->SetPositionAndSize(0, 240, 80, 40);
		nextButton->SetZ(0.06f);
		nextButton->SetBorderSize(1.f);
		nextButton->GetClickHelper().AddOnClickListener([this](auto, const MousePoint & ms, int mouseButton) {
			//this->GetFramework()->SwitchScene();
		});
		
		std::shared_ptr<Button> buttonux = std::make_shared<Button>(1, XGF::string(L"Switch Mode"));
		mBtnGroup->AddChild(buttonux);

		buttonux->SetPositionAndSize(10, 100, 60, 40);
		buttonux->SetBorderSize(2);
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		buttonux->SetZ(0.07f);
		buttonux->GetClickHelper().AddOnClickListener([this, &gdi](auto, const MousePoint & ms, int mouseButton) {
			if (gdi.GetDisplayMode() == DisplayMode::Borderless)
				gdi.SetDisplayMode(DisplayMode::Windowed, 0, 0, 600, 400, false);
			else if (gdi.GetDisplayMode() == DisplayMode::FullScreen)
				gdi.SetDisplayMode(DisplayMode::Borderless, 0, 0, 1920, 1080, true);
			else
				gdi.SetDisplayMode(DisplayMode::FullScreen, 0, 0, 1920, 1080, true);
		});

		std::shared_ptr<EditText> edit1 = std::make_shared<EditText>(10);
		std::shared_ptr<EditText> edit2 = std::make_shared<EditText>(11);

		edit1->SetPositionAndSize(300, 20, 200, 40);
		edit1->SetBorderSize(1);
		edit1->SetZ(0.08f);

		edit2->SetPositionAndSize(140, 20, 120, 40);
		edit2->SetBorderSize(2);
		edit2->SetZ(0.08f);

		GetRootContainer().AddChild(edit1);
		GetRootContainer().AddChild(edit2);

		//resource loading
		auto res = std::vector<ResourceInfo>();
		res.push_back(ResourceInfo(L"_normal.png", L"normal"));
		res.push_back(ResourceInfo(L"_hover.png", L"hover"));
		res.push_back(ResourceInfo(L"_activate.png", L"activate"));
		res.push_back(ResourceInfo(L"cursor.png", L"cursor"));

		mTextureResourceManager.LoadResourceAsync(res, &GetFramework().GetThread(), [this](std::vector<ResourceInfo> ress, int success) {
			if (success < 4) return;
			auto t1 = Texture(*mTextureResourceManager.GetResourceByAlias(L"normal"));
			auto t2 = Texture(*mTextureResourceManager.GetResourceByAlias(L"hover"));
			auto t3 = Texture(*mTextureResourceManager.GetResourceByAlias(L"activate"));
			auto cursor = mTextureResourceManager.GetResourceByAlias(L"cursor");
			t1.Set9PathBorderSize(3);
			t2.Set9PathBorderSize(3);
			t3.Set9PathBorderSize(3);
			std::static_pointer_cast<Button>(GetRootContainer().GetActorById(1, true))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));
			std::static_pointer_cast<Button>(GetRootContainer().GetActorById(2, true))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));

			t1.Set9PathBorderSize(3.5);
			t2.Set9PathBorderSize(3.5);
			t3.Set9PathBorderSize(3.5);
			std::static_pointer_cast<Control>(GetRootContainer().GetActorById(10))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));
			std::static_pointer_cast<Control>(GetRootContainer().GetActorById(11))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));

			GetFramework().GetInputManager().GetCursor().SetStaticTexture(cursor);
			GetFramework().GetInputManager().SetMouseMode(MouseMode::Custom);
		});
		mBtnGroup->GetTransform().TranslateToX(100);
	};
	virtual void OnDestroy() override
	{
		mTextRenderer_s.Shutdown();
		mUITextRenderer.Shutdown();
		mTextRenderer_b.Shutdown();

		mFont.Shutdown();
		mFont_s.Shutdown();
		mFont_b.Shutdown();

		mTextureResourceManager.ReleaseAllResource();
	};
	virtual void Render(float deltaTime) override
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

		gdi.Clear(Color(0.5f, 0.5f, 0.5f, 1.0f));
		WVPMatrix wvp2D;
		mCamera2D.GetCameraMatrix(wvp2D);

		auto debug = DebugInscriber::GetInstance();
		mTextRenderer_s.Begin(wvp2D);
		auto height = gdi.GetHeight();
		std::wstring costStr = fmt::format(L"IndicesRenderCountPerFrame:{0}", debug->GetIndicesRenderCountPerFrame());
		auto & framework = GetFramework();
		mTextRenderer_s.DrawString(costStr.c_str(), 2.f, static_cast<float>(height - 80));
		costStr = fmt::format(L"IndicesRenderCountPerSecond:{0}", debug->GetIndicesRenderCountPerSecond());
		mTextRenderer_s.DrawString(costStr.c_str(), 300.f, static_cast<float>(height - 80));
		costStr = fmt::format(L"VerticesRenderCountPerFrame:{0}", debug->GetVerticesRenderCountPerFrame());
		mTextRenderer_s.DrawString(costStr.c_str(), 2.f, static_cast<float>(height - 60));
		costStr = fmt::format(L"VerticesRenderCountPerSecond:{0}", debug->GetVerticesRenderCountPerSecond());
		mTextRenderer_s.DrawString(costStr.c_str(), 300.f, static_cast<float>(height - 60));
		costStr = fmt::format(L"CallBatchPerFrame:{0}", debug->GetCallBatchPerFrame());
		mTextRenderer_s.DrawString(costStr.c_str(), 2.f, static_cast<float>(height  - 40));
		costStr = fmt::format(L"CallBatchPerSecond:{0}", debug->GetCallBatchPerSecond());
		mTextRenderer_s.DrawString(costStr.c_str(), 300.f, static_cast<float>(height - 40));
		costStr = fmt::format(L"PolygonRenderCountPerSecond:{0}", debug->GetPolygonRenderCountPerSecond());
		mTextRenderer_s.DrawString(costStr.c_str(), 300.f, static_cast<float>(height - 20));
		costStr = fmt::format(L"PolygonRenderCountPerFrame:{0}", debug->GetPolygonRenderCountPerFrame());
		mTextRenderer_s.DrawString(costStr.c_str(), 2.f, static_cast<float>(height - 20));

		mTextRenderer_s.End();

		mTextRenderer_b.Begin(wvp2D);

		std::wstring fps_str = fmt::format(L"FPS:{0}\nFC:{1}ms", debug->GetAverageFPS(), debug->GetFrameCost());
		mTextRenderer_b.DrawString(fps_str.c_str(), SM::Color(0.2f, 0.2f, 0.8f, 1.0f), 4, 4);
		mTextRenderer_b.End();
		RenderUI(wvp2D);
	};
	virtual void Update(float deltaTime) override
	{
		mCamera2D.Update();
		if(mDirection)
		{
			mBtnGroup->GetTransform().TranslateX(-deltaTime * 20);
			if (mBtnGroup->GetTransform().GetTranslateX() < 50)
			{
				mDirection = false;
			}
		}
		else
		{
			mBtnGroup->GetTransform().TranslateX(deltaTime * 20);
			if (mBtnGroup->GetTransform().GetTranslateX() > 150)
			{
				mDirection = true;
			}
		}
		auto & ip = GetFramework().GetInputManager();
		if (ip.IskeyDown(DIK_ESCAPE))
			GetFramework().Exit(0);
	};
	virtual void OnSize(int ClientX, int ClientY) override
	{
		mCamera2D.UpdateProject(ClientX, ClientY);
	};

private:
	OrthoCamera mCamera2D;

	Font mFont;
	TextRenderer mUITextRenderer;
	Font mFont_s;
	TextRenderer mTextRenderer_s;
	Font mFont_b;
	TextRenderer mTextRenderer_b;
	shared_ptr<Container> mBtnGroup;

	TextureResourceManager mTextureResourceManager;

	bool mDirection = true;
};


int RunGame(HINSTANCE hInstance)
{
	Application app;
	XGFramework framework;
	GDI gdi;

	auto gameScene = std::make_shared<GameScene>();
	framework.SetOnCloseListener(XGFramework::AutoClose());

	WindowProperty windowProperty;
	windowProperty.title = L"UI";
	windowProperty.className = L"UI";
	windowProperty.ICON = 0;
	windowProperty.SICON = 0;
	windowProperty.canResize = true;
	windowProperty.point = { 300, 100 };
	windowProperty.size = { 600, 400 };

	return app.CreateWindowsAndRunApplication(framework, gdi, hInstance, windowProperty, gameScene);
}