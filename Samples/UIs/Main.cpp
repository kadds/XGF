#define _XGF_DEBUG_ALLOC
#include "../../XGF/Include/XGF.h"
#include "./../../XGF/Extra/imgui/imgui_impl.hpp"
#include <fmt/format.h>
#include <memory>
using namespace std;
using namespace XGF;
#include <iomanip>
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#define new  new(_CLIENT_BLOCK, __FILE__, __LINE__)  

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
		imgui.Initialize();
		auto& context = Context::Current();
		mFont.Initialize(Tools::GetFontPath(L"msyh"), 16);
		//context.QueryFramework().GetInputManager().SwitchToDInput();

		//resource loading
		auto res = std::vector<ResourceInfo>();
		res.push_back(ResourceInfo(L"cursor.png", L"cursor"));

		mTextureResourceManager.LoadResourceAsync(res, &Context::Current().QueryGameThread(), [this](std::vector<ResourceInfo> ress, int success) {
			if (success < 1) return;
			auto cursor = mTextureResourceManager.GetResourceByAlias(L"cursor");
		});
		
		this->GetRootContainer().GetEventDispatcher().InsertAllEventListener(bind(&UI::Imgui::OnInputEvent, &imgui, placeholders::_1));

	};
	virtual void OnDestroy() override
	{
		imgui.Shutdown();
		mFont.Shutdown();
		mTextureResourceManager.ReleaseAllResource();
	};
	virtual void Render() override
	{
		auto & context = Context::Current();
		auto & gdi = context.QueryGraphicsDeviceInterface();
		context.QueryRenderer().PushDefaultFrameTarget();
		context.QueryRenderer().Clear(Color(0.5, 0.5, 0.5, 1));
		imgui.Begin();
		ImGui::ShowDemoWindow();
		auto debug = DebugInscriber::GetInstance();
		auto height = gdi.GetHeight();
		std::string costStr = fmt::format("IndicesRenderCountPerFrame:{0}\n", debug->GetIndicesRenderCountPerFrame());
		auto & framework = GetFramework();
		costStr += fmt::format("IndicesRenderCountPerSecond:{0}\n", debug->GetIndicesRenderCountPerSecond());
		costStr += fmt::format("VerticesRenderCountPerFrame:{0}\n", debug->GetVerticesRenderCountPerFrame());
		costStr += fmt::format("VerticesRenderCountPerSecond:{0}\n", debug->GetVerticesRenderCountPerSecond());
		costStr += fmt::format("CallBatchPerFrame:{0}\n", debug->GetCallBatchPerFrame());
		costStr += fmt::format("CallBatchPerSecond:{0}\n", debug->GetCallBatchPerSecond());
		costStr += fmt::format("PolygonRenderCountPerSecond:{0}\n", debug->GetPolygonRenderCountPerSecond());
		costStr += fmt::format("PolygonRenderCountPerFrame:{0}\n", debug->GetPolygonRenderCountPerFrame());


		std::string fps_str = fmt::format("FPS:{0}\nFC:{1}ms", debug->GetAverageFPS(), debug->GetFrameCost());
		
		ImGui::Begin("info window");
		ImGui::Text(fps_str.c_str());
		ImGui::Text(costStr.c_str());
		ImGui::End();
		imgui.End();
	};
	virtual void Update(float deltaTime) override
	{
		auto & ip = GetFramework().GetInputManager();
		if (ip.IskeyDown(Input::KeyBoardKey::Escape))
			GetFramework().Exit(0);
	};
	virtual void OnSize(int cx, int cy) override
	{
		Context::Current().QueryRenderer().WaitFrame();
		imgui.Resize(cx, cy);
	};

private:
	TextureResourceManager mTextureResourceManager;
	Font mFont;
	UI::Imgui imgui;
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