#define _XGF_DEBUG_ALLOC
#include "./../../XGF/Include/XGF.h"
#include <fmt/format.h>
#include <iostream>
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
	
	void MyRender()
	{
		auto & context = Context::Current();
		mShapeRenderer.DrawCircle(240, 240, 40, GetPrecision(40, 5), 0.01f, Color(1.f, 1.f, 0.f, 1.f), Color(0.f, 1.f, 0.f, 1.f));
		mTextRenderer.DrawString(L"Circle", 200, 180);
		mShapeRenderer.DrawRectangleC(50, 50, 100, 100, 0.1f, Color(1.f, 0.f, 0.0f, 1.f), 8.f, Color(0.f, 0.f, 1.f, 1.f), Color(0.f, 1.f, 0.f, 1.f));
		mTextRenderer.DrawString(L"RectangleB", 50, 100);
		mShapeRenderer.DrawRectangle(250, 50, 100, 100, 0.1f, Color(0.f, 0.2f, 0.7f, 1.f));
		mTextRenderer.DrawString(L"Rectangle", 280, 100);
		mShapeRenderer.DrawLine(160, 0, 160, 180, 0.2f, Color(1.f, 1.f, 1.f, 1.f));
		mTextRenderer.DrawString(L"Line", 140, 188);
		mShapeRenderer.DrawTriangle(Position(40, 200), Position(70, 210), Position(40, 240), 0.1f, Color(0.4f, 0.8f, 0.1f, 1.f));
		mTextRenderer.DrawString(L"Triangle", 40, 220);
		Position p[5] = { { 0,50 },{ 50,50 },{ 40,100 },{ 100,100 },{ 100,80 } };
		mShapeRenderer.DrawLineList(*p, 5, 0.001f, Color(0.0, 0.0, 0.0, 1.0));
		mTextRenderer.DrawString(L"LineList", 4, 70);
		Position p2[5] = { { 0,150 },{ 50,120 },{ 40,200 },{ 100,200 },{ 100,220 } };
		mShapeRenderer.DrawPolygonList(*p2, 5, 0.1f, Color(0.0f, 0.1f, 0.6f, 1.0f));
		mTextRenderer.DrawString(L"PolygonList", 40, 172);
		mTextRenderer.DrawString(L"A RenderToTexture(RTT) Demo", 125, 35);
		mTextRenderer.Flush();
	};
	virtual void OnCreate() override
	{
		auto res = std::vector<ResourceInfo>();
		res.push_back(ResourceInfo(L"cursor.png", L"cursor"));
		mTextureResourceManager.LoadResource(res);
		GetFramework().GetInputManager().GetCursor().SetStaticTexture(mTextureResourceManager.GetResourceByAlias(L"cursor"));
		GetFramework().GetInputManager().SetMouseMode(MouseMode::Custom);

		mShapeRenderer.Initialize();
		mFont.Initialize(Tools::GetFontPath(L"msyh"), 16);
		mTextRenderer.Initialize(&mFont, 240);
		auto & context = Context::Current();
		mRenderResource.ReCreate(SystemShaders::GetBasicShaders(SystemShaders::BasicShader_Texture));
		mTextureRenderState.GetBlendState().GetRenderTarget(0).SetBlendEnable(true);
		mTextureRenderState.GetDepthStencilState().SetDepthEnable(false);
		auto & gdi = context.QueryGraphicsDeviceInterface();
		mRenderToTextureTarget.Initialize(gdi.GetWidth(), gdi.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DepthStencilFormat::Depth24FloatStencil8Uint, 0, 1);
		
		mRc.SetZ(0.001f);
		mTime = 0.f;
	};
	virtual void OnDestroy() override
	{
		mRenderToTextureTarget.Shutdown();
		mRenderResource.Clear();
		mTextRenderer.Shutdown();
		mFont.Shutdown();
		mShapeRenderer.Shutdown();
		mTextureResourceManager.ReleaseAllResource();
	};
	virtual void Render() override
	{
		auto & context = Context::Current();
		context.QueryRenderer().PushDefaultFrameTarget();
		context.QueryRenderer().Clear(Color(0.5, 0.5, 0.5, 1));
		WVPMatrix wvp;
		mCamera.GetCameraMatrix(wvp);
		mShapeRenderer.Begin(wvp);
		mTextRenderer.Begin(wvp);
		MyRender();

		context.QueryRenderer().PushFrameTarget(&mRenderToTextureTarget);
		context.QueryRenderer().Clear(Color(0.3f, 0.3f, 0.3f, 1));
		MyRender();
		context.QueryRenderer().PopFrameTarget();
		

		PolygonPleTextureBinder textureBinder(4);
		textureBinder.GetData(1).x = textureBinder.GetData(0).x = 0.f;
		textureBinder.GetData(2).x = textureBinder.GetData(3).x = 1.f;
		textureBinder.GetData(1).y = textureBinder.GetData(2).y = 1.f;
		textureBinder.GetData(3).y = textureBinder.GetData(0).y = 0.f;

		BindingBridge bb;
		bb.AddBinder(mRc.mPolygon);
		bb.AddBinder(std::shared_ptr<PolygonPleTextureBinder>(&textureBinder, [](const PolygonPleTextureBinder *) {}));

		auto debug = DebugInscriber::GetInstance();
		std::wstring str = fmt::format(L"FPS:{0}\nFC:{1}ms", debug->GetAverageFPS(), debug->GetFrameCost());
		mTextRenderer.DrawString(str, 4, 4);

		mTextRenderer.End();
		mShapeRenderer.End();

		mRenderResource.SetConstantBuffer<VertexShader>(0, 0, wvp);
		mRenderResource.SetConstantBuffer<PixelShader>(0, 0, Color(1.f, 1.f, 1.f, 1.f));
		mRenderResource.SetTexture<PixelShader>(0, mRenderToTextureTarget.GetTexture(0));
		if(mRenderToTextureTarget.GetTexture(0))
			Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(bb, *mRc.mPolygonPleIndex.get(), 
				RenderStage(mTextureRenderState, mRenderResource)));

	};
	virtual void Update(float deltaTime) override
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		if (GetFramework().GetInputManager().IskeyDown(DIK_F11) && mTime >= 0.3f)
		{
			if (!gdi.IsDisplayMode(DisplayMode::Borderless))
				gdi.SetDisplayMode(DisplayMode::Borderless, 0, 0, 1920, 1080, true, true);
			else
				gdi.SetDisplayMode(DisplayMode::Windowed, 400, 400, 600, 400, false, true);
			mTime = 0.f;
		}
		else if (mTime >= 0.f)
		{
			mTime += deltaTime;
			if (mTime > 50000.f) mTime = 1.f;
		}
		mCamera.Update();
	};
	virtual void OnSize(int cx, int cy) override
	{
		mCamera.UpdateProject(cx, cy);
		auto & renderer = Context::Current().QueryRenderer();
		renderer.WaitFrame();
		mRenderToTextureTarget.Shutdown();
		mRenderToTextureTarget.Initialize(cx, cy, DXGI_FORMAT_R8G8B8A8_UNORM, DepthStencilFormat::Depth24FloatStencil8Uint, 0, 1);
		
		mRc.SetPositionAndSize(0.f, cy * 2 / 3.f, cx / 3.f, cy / 3.f);
	};

private:
	ShapeRenderer mShapeRenderer;
	OrthoCamera mCamera;
	FrameBuffer mRenderToTextureTarget;
	TextureResourceManager mTextureResourceManager;
	Font mFont;
	TextRenderer mTextRenderer;

	RenderState mTextureRenderState;
	RenderResource mRenderResource;

	Shape::Rectangle mRc;

	Texture mTexture;

	float mTime;
};


int RunGame(HINSTANCE hInstance)
{
	Application app;
	XGFramework framework;
	GDI gdi;

	auto gameScene = std::make_shared<GameScene>();
	framework.SetOnCloseListener(XGFramework::AutoClose());

	WindowProperty windowProperty;
	windowProperty.title = L"BaseShapes";
	windowProperty.className = L"BaseShapes";
	windowProperty.ICON = 0;
	windowProperty.SICON = 0;
	windowProperty.canResize = false;
	windowProperty.point = {300, 100};
	windowProperty.size = { 600, 400 };

	return app.CreateWindowsAndRunApplication(framework, gdi, hInstance, windowProperty, gameScene);
}