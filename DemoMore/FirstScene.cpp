#include "FirstScene.hpp"
#include "SecondScene.hpp"
#include <memory>
#include <iomanip>
FirstScene::FirstScene()
{
}


FirstScene::~FirstScene()
{
}

void FirstScene::OnCreate()
{
	wchar_t buffer[MAX_PATH];
	char cbuffer[MAX_PATH];
	Tools::GetInstance()->GetFontPath("Dengb.ttf", cbuffer, MAX_PATH);
	mAxisRenderer.Initialize(mFramework->GetGDI());
	
	mBatch3D.Initialize(mFramework->GetGDI(), ConstantData::GetInstance().GetPCShaders(), 100, 100);
	mCube.SetPositionAndSize(-1, -1, -1, 2, 2, 2);

	mFont.Initialize(mFramework->GetGDI(), cbuffer, 16);
    mFont_s.Initialize(mFramework->GetGDI(), cbuffer, 12);
	mFont_b.Initialize(mFramework->GetGDI(), cbuffer, 20);
	mUITextRenderer.Initialize(mFramework->GetGDI(), &mFont, 1000);
    mTextRenderer_s.Initialize(mFramework->GetGDI(), &mFont_s, 1000);
	mTextRenderer_b.Initialize(mFramework->GetGDI(), &mFont_b, 200);
	mFramework->GetUIBatches().SetTextRenderer(BATCHES_TEXTRENDERER_DEFAULT_SIZE, &mUITextRenderer);

	mLb.SetText(L"Direct3D11");
    mLb.SetPositionAndSize(50, 200, 150, 40);
	mLb.SetZ(0.4f);
	//mLb.GetTransform().mRotation = Point(0, 0, 0.4);
	GetRootContainer().AddChild(mLb);
	mLb.GetClickHelper().AddOnClickListener([this](const MousePoint & ms, int mouseButton) {
		AsyncTask::NewTask(mFramework->GetTheard(), [this](AsyncTask * asyn) {
			MessageBox(NULL, L"YOU CLICK Label!!",L"Exe",0);
			asyn->Finish(0, 0);
		});
	});
	mLxb.SetText(L"X Game Render Framework\nClick here switch next scene.");
    mLxb.SetPositionAndSize(0, 240, 240, 40);
	GetRootContainer().AddChild(mLxb);
	mLxb.SetZ(0.06f);
	mLxb.GetClickHelper().AddOnClickListener([this](const MousePoint & ms, int mouseButton) {
		this->GetFramework()->SwitchScene(new SecondScene());
	});

	mBt.SetPositionAndSize(10,100,60,30);
	btNormal.LoadWIC(mFramework->GetGDI(), GetFilePath(L"a.png", buffer, 100));
    btMove.LoadWIC(mFramework->GetGDI(), GetFilePath(L"a1.png", buffer, 100));
    btPress.LoadWIC(mFramework->GetGDI(), GetFilePath(L"a2.png", buffer, 100));

	mBt.GetClickHelper().AddOnClickListener([this](const MousePoint & ms, int mouseButton) {
		if (mFramework->GetGDI()->GetDisplayMode() == DisplayMode::Borderless)
				mFramework->GetGDI()->SetDisplayMode(DisplayMode::Windowed, 0, 0, 600, 400, false);
			else if(mFramework->GetGDI()->GetDisplayMode() == DisplayMode::FullScreen)
				mFramework->GetGDI()->SetDisplayMode(DisplayMode::Borderless ,0 ,0 , 1920, 1080, true);
			else
				mFramework->GetGDI()->SetDisplayMode(DisplayMode::FullScreen, 0, 0, 1920, 1080, true);
	});
	//mBt.AddOnClickListener([=](const MousePoint &mm, int pk) {
	//	mBt.StartAction();
	//	if (mFramework->GetGDI()->GetDisplayMode() == DisplayMode::Borderless)
	//		mFramework->GetGDI()->SetDisplayMode(DisplayMode::Windowed, 0, 0, 600, 400, false);
	//	else if(mFramework->GetGDI()->GetDisplayMode() == DisplayMode::FullScreen)
	//		mFramework->GetGDI()->SetDisplayMode(DisplayMode::Borderless ,0 ,0 , 1920, 1080, true);
	//	else
	//		mFramework->GetGDI()->SetDisplayMode(DisplayMode::FullScreen, 0, 0, 1920, 1080, true);
		//AsyncTask::NewTask(mFramework->GetTheard(), [this](AsyncTask * asyn) {
			
			//MessageBox(NULL, L"YOU CLICK BUTTOM!!",L"E",0);
			//asyn->Finish(0, 0);
		//});
		
   // });
	mBt.SetZ(0.07f);
	mBt.SetSkin(Skin::CreateFromTextures(&btNormal, &btMove, &btPress));
	GetRootContainer().AddChild(mBt);
	mEdit.SetPositionAndSize(300, 20, 200, 40);
	mEdit.SetBorderSize(2);
	mEdit.SetZ(0.08f);
	
	mEdit2.SetPositionAndSize(140, 20, 120, 40);
	mEdit2.SetBorderSize(2);
	mEdit2.SetZ(0.08f);

	GetRootContainer().AddChild(mEdit2);
	GetRootContainer().AddChild(mEdit);

	mFramework->GetInputManager()->GetCursor()->SetStaticTexture(mFramework->GetGDI(), GetFilePath(L"cursor.png", buffer, 100));

	mCamera.SetPos(XMFLOAT3(1,1,-10));
}

void FirstScene::OnDestroy()
{
    mTextRenderer_s.Shutdown();
	mUITextRenderer.Shutdown();
	mTextRenderer_b.Shutdown();

	mFont.Shutdown();
	mFont_s.Shutdown();
	mFont_b.Shutdown();

	mAxisRenderer.Shutdown();
    btNormal.Release();
    btMove.Release();
    btPress.Release();

	mBatch3D.Shutdown();
}

void FirstScene::Render(float deltaTime)
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
	mTextRenderer_s.DrawString(str.str().c_str(), 2, mFramework->GetWindowsHeight() - 80);
	str.clear();
	str.str(L"");
	str << L"IndicesRenderCountPerSecond:" << debug->GetIndicesRenderCountPerSecond();
	mTextRenderer_s.DrawString(str.str().c_str(), 300, mFramework->GetWindowsHeight() - 80);
	str.clear();
	str.str(L"");
	str << L"VerticesRenderCountPerFrame:" << debug->GetVerticesRenderCountPerFrame();
	mTextRenderer_s.DrawString(str.str().c_str(), 2, mFramework->GetWindowsHeight() - 60);
	str.clear();
	str.str(L"");
	str << L"VerticesRenderCountPerSecond:" << debug->GetVerticesRenderCountPerSecond();
	mTextRenderer_s.DrawString(str.str().c_str(), 300, mFramework->GetWindowsHeight() - 60);
	str.clear();
	str.str(L"");
	str << L"CallBatchPerFrame:" << debug->GetCallBatchPerFrame();
	mTextRenderer_s.DrawString(str.str().c_str(), 2, mFramework->GetWindowsHeight() - 40);
	str.clear();
	str.str(L"");
	str << L"CallBatchPerSecond:" << debug->GetCallBatchPerSecond();
	mTextRenderer_s.DrawString(str.str().c_str(), 300, mFramework->GetWindowsHeight() - 40);
	str.clear();
	str.str(L"");
	str << L"PolygonRenderCountPerSecond:" << debug->GetPolygonRenderCountPerSecond();
	mTextRenderer_s.DrawString(str.str().c_str(), 300, mFramework->GetWindowsHeight() - 20);
	str.clear();
	str.str(L"");
	str << L"PolygonRenderCountPerFrame:" << debug->GetPolygonRenderCountPerFrame();
	mTextRenderer_s.DrawString(str.str().c_str(), 2, mFramework->GetWindowsHeight() - 20);

	
	
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
	mTextRenderer_b.DrawString(str.str().c_str(), Color(0.2f,0.2f,0.8f,1.0f),4, 4);
	mTextRenderer_b.End();
}

void FirstScene::Updata(float deltaTime)
{
    mCamera.Updata();
    mCamera2D.Updata();
	auto ip = GetFramework()->GetInputManager();
	if (ip->IskeyDowm(DIK_ESCAPE))
		GetFramework()->Exit(0);
}

void FirstScene::OnSize(int ClientX, int ClientY)
{
	mCamera2D.UpdataProject(ClientX, ClientY);
	mCamera.UpdataProject(ClientX, ClientY);
}

