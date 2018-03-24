#define _XGF_DEBUG_ALLOC
#include "../../XGF/Include/XGF.h"
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
		char cbuffer[MAX_PATH];
		Tools::GetFontPath("Dengb.ttf", cbuffer, MAX_PATH);
		mAxisRenderer.Initialize(gdi);

		mBatch3D.Initialize(gdi, ConstantData::GetInstance().GetPCShaders(), 100, 100);
		mCube.SetPositionAndSize(-1, -1, -1, 2, 2, 2);

		mFont.Initialize(gdi, cbuffer, 16);
		mFont_s.Initialize(gdi, cbuffer, 12);
		mFont_b.Initialize(gdi, cbuffer, 20);
		mUITextRenderer.Initialize(gdi, &mFont, 1000);
		mTextRenderer_s.Initialize(gdi, &mFont_s, 1000);
		mTextRenderer_b.Initialize(gdi, &mFont_b, 200);
		mFramework->GetUIBatches().SetTextRenderer(FontSize::Default, &mUITextRenderer);

		std::shared_ptr<Label> label = std::make_shared<Label>(0, L"Direct3D11");
		label->SetPositionAndSize(200, 200, 100, 20);
		label->SetZ(0.4f);
		GetRootContainer().AddChild(label);

		label->GetClickHelper().AddOnClickListener([this, label](const MousePoint & ms, int mouseButton) {
			AsyncTask::NewTask(mFramework->GetTheard(), [this](AsyncTask * asyn) {
				MessageBox(NULL, L"YOU CLICK Label!!", L"Exe", 0);
				asyn->Finish(0, 0);
			});
			label->mTransform.AddTranslationAction(Action::Make(Point(200, 2, 0), 2.0, false, LinearInterpolator::GetInterpolator()));
		});
		std::shared_ptr<Button> nextButton = std::make_shared<Button>(2, L"switch to next scene.");
		GetRootContainer().AddChild(nextButton);
		nextButton->SetPositionAndSize(0, 240, 80, 40);
		nextButton->SetZ(0.06f);
		nextButton->SetBorderSize(1.f);
		nextButton->GetClickHelper().AddOnClickListener([this](const MousePoint & ms, int mouseButton) {
			//this->GetFramework()->SwitchScene();
		});

		std::shared_ptr<Button> buttonux = std::make_shared<Button>(1, string(L"Switch Mode"));
		GetRootContainer().AddChild(buttonux);
		
		buttonux->SetPositionAndSize(10, 100, 60, 40);
		buttonux->SetBorderSize(2);
		buttonux->SetZ(0.07f);
		buttonux->GetClickHelper().AddOnClickListener([this](const MousePoint & ms, int mouseButton) {
			auto gdi = this->GetFramework()->GetGDI();
			if (gdi->GetDisplayMode() == DisplayMode::Borderless)
				gdi->SetDisplayMode(DisplayMode::Windowed, 0, 0, 600, 400, false);
			else if (gdi->GetDisplayMode() == DisplayMode::FullScreen)
				gdi->SetDisplayMode(DisplayMode::Borderless, 0, 0, 1920, 1080, true);
			else
				gdi->SetDisplayMode(DisplayMode::FullScreen, 0, 0, 1920, 1080, true);
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

		mCamera.SetPos(XMFLOAT3(1, 1, -10));

		//resource loading
		auto res = std::vector<ResourceInfo>();
		res.push_back(ResourceInfo(L"_normal.png", L"normal"));
		res.push_back(ResourceInfo(L"_hover.png", L"hover"));
		res.push_back(ResourceInfo(L"_activate.png", L"activate"));
		res.push_back(ResourceInfo(L"cursor.png", L"cursor"));

		mTextureResourceManager.LoadResourceAsync(gdi, res, mFramework->GetTheard(), [this](std::vector<ResourceInfo> ress, int success) {
			auto t1 = Texture(*mTextureResourceManager.GetResourceByAlias(L"normal"));
			auto t2 = Texture(*mTextureResourceManager.GetResourceByAlias(L"hover"));
			auto t3 = Texture(*mTextureResourceManager.GetResourceByAlias(L"activate"));
			auto cursor = mTextureResourceManager.GetResourceByAlias(L"cursor");
			t1.Set9PathBorderSize(3);
			t2.Set9PathBorderSize(3);
			t3.Set9PathBorderSize(3);
			std::static_pointer_cast<Button>(GetRootContainer().GetActorById(1))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));
			std::static_pointer_cast<Button>(GetRootContainer().GetActorById(2))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));

			t1.Set9PathBorderSize(3.5);
			t2.Set9PathBorderSize(3.5);
			t3.Set9PathBorderSize(3.5);
			std::static_pointer_cast<Control>(GetRootContainer().GetActorById(10))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));
			std::static_pointer_cast<Control>(GetRootContainer().GetActorById(11))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));

			mFramework->GetInputManager()->GetCursor()->SetStaticTexture(cursor);
		});
	};
	virtual void OnDestroy() override
	{
		mTextRenderer_s.Shutdown();
		mUITextRenderer.Shutdown();
		mTextRenderer_b.Shutdown();

		mFont.Shutdown();
		mFont_s.Shutdown();
		mFont_b.Shutdown();

		mAxisRenderer.Shutdown();

		mBatch3D.Shutdown();
		mTextureResourceManager.ReleaseAllResource();
	};
	virtual void Render(float deltaTime) override
	{
		Clear(Color(0.5f, 0.5f, 0.5f, 1.0f));
		WVPMatrix wvp2D, wvp3D;
		mCamera2D.GetCameraMatrix(wvp2D);
		mCamera.GetCameraMatrix(wvp3D);

		auto debug = DebugInscriber::GetInstance();
		mTextRenderer_s.Begin(wvp2D);

		std::wstringstream str;
		str << std::fixed << std::setprecision(1);
		str << L"IndicesRenderCountPerFrame:" << debug->GetIndicesRenderCountPerFrame();
		mTextRenderer_s.DrawString(str.str().c_str(), 2.f, static_cast<float>(mFramework->GetWindowsHeight() - 80));
		str.clear();
		str.str(L"");
		str << L"IndicesRenderCountPerSecond:" << debug->GetIndicesRenderCountPerSecond();
		mTextRenderer_s.DrawString(str.str().c_str(), 300.f, static_cast<float>(mFramework->GetWindowsHeight() - 80));
		str.clear();
		str.str(L"");
		str << L"VerticesRenderCountPerFrame:" << debug->GetVerticesRenderCountPerFrame();
		mTextRenderer_s.DrawString(str.str().c_str(), 2.f, static_cast<float>(mFramework->GetWindowsHeight() - 60));
		str.clear();
		str.str(L"");
		str << L"VerticesRenderCountPerSecond:" << debug->GetVerticesRenderCountPerSecond();
		mTextRenderer_s.DrawString(str.str().c_str(), 300.f, static_cast<float>(mFramework->GetWindowsHeight() - 60));
		str.clear();
		str.str(L"");
		str << L"CallBatchPerFrame:" << debug->GetCallBatchPerFrame();
		mTextRenderer_s.DrawString(str.str().c_str(), 2.f, static_cast<float>(mFramework->GetWindowsHeight() - 40));
		str.clear();
		str.str(L"");
		str << L"CallBatchPerSecond:" << debug->GetCallBatchPerSecond();
		mTextRenderer_s.DrawString(str.str().c_str(), 300.f, static_cast<float>(mFramework->GetWindowsHeight() - 40));
		str.clear();
		str.str(L"");
		str << L"PolygonRenderCountPerSecond:" << debug->GetPolygonRenderCountPerSecond();
		mTextRenderer_s.DrawString(str.str().c_str(), 300.f, static_cast<float>(mFramework->GetWindowsHeight() - 20));
		str.clear();
		str.str(L"");
		str << L"PolygonRenderCountPerFrame:" << debug->GetPolygonRenderCountPerFrame();
		mTextRenderer_s.DrawString(str.str().c_str(), 2.f, static_cast<float>(mFramework->GetWindowsHeight() - 20));



		mAxisRenderer.Begin(wvp3D);
		mAxisRenderer.SetAxisXColor(Color(1.f, 0.f, 0.f, 1.0), Color(0.5f, 0.f, 0.f, 1.f));
		mAxisRenderer.SetAxisYColor(Color(0.f, 1.f, 0.f, 1.0), Color(0.f, 0.5f, 0.f, 1.f));
		mAxisRenderer.SetAxisZColor(Color(0.f, 0.f, 1.f, 1.0), Color(0.f, 0.f, 0.5f, 1.f));
		mAxisRenderer.DrawAxis();
		mAxisRenderer.End();


		PolygonPleColorBinder cb(mCube.mPolygon.mCount);
		cb.mColor[0] = Color(1.0, 0.0, 0.0, 1.0);
		cb.mColor[1] = Color(1.0, 1.0, 0.0, 1.0);
		cb.mColor[2] = Color(1.0, 1.0, 1.0, 1.0);
		cb.mColor[3] = Color(0.0, 1.0, 1.0, 1.0);
		cb.mColor[4] = Color(0.0, 1.0, 0.0, 1.0);
		cb.mColor[5] = Color(0.0, 0.0, 1.0, 1.0);
		cb.mColor[6] = Color(0.0, 0.0, 0.0, 1.0);
		cb.mColor[7] = Color(1.0, 0.0, 1.0, 1.0);
		BindingBridge bbr;
		bbr.AddBinder(mCube.mPolygon);
		bbr.AddBinder(cb);
		mBatch3D.GetShaderStage()->SetVSConstantBuffer(0, &wvp3D);
		mBatch3D.Begin();
		mBatch3D.DrawPolygon(mCube.mPolygonPleIndex, bbr);
		mBatch3D.End();
		mTextRenderer_s.End();

		mTextRenderer_b.Begin(wvp2D);

		str.clear();
		str.str(L"");
		str << std::fixed << std::setprecision(1) << L"FPS:" << debug->GetAverageFPS() << "\n" << L"FC:" << std::setprecision(4) << debug->GetFrameCost() << "ms";
		mTextRenderer_b.DrawString(str.str().c_str(), Color(0.2f, 0.2f, 0.8f, 1.0f), 4, 4);
		mTextRenderer_b.End();
	};
	virtual void Update(float deltaTime) override
	{
		mCamera.Update();
		mCamera2D.Update();
		auto ip = GetFramework()->GetInputManager();
		if (ip->IskeyDowm(DIK_ESCAPE))
			GetFramework()->Exit(0);
	};
	virtual void OnSize(int ClientX, int ClientY) override
	{
		mCamera2D.UpdateProject(ClientX, ClientY);
		mCamera.UpdateProject(ClientX, ClientY);
	};

private:
	PerspectiveCamera mCamera;
	OrthoCamera mCamera2D;
	AxisRenderer mAxisRenderer;

	Batch mBatch3D;
	Shaders mShader3D;
	Shape::Cube mCube;
	Font mFont;
	TextRenderer mUITextRenderer;
	Font mFont_s;
	TextRenderer mTextRenderer_s;
	Font mFont_b;
	TextRenderer mTextRenderer_b;

	TextureResourceManager mTextureResourceManager;
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