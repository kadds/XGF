#include "FirstScene.hpp"
#include "SecondScene.hpp"
#include <memory>
#include <iomanip>

void Deleter(Control * c) {
	delete c;
}

FirstScene::FirstScene()
{
}


FirstScene::~FirstScene()
{
}

void FirstScene::OnCreate()
{
	char cbuffer[MAX_PATH];
	Tools::GetFontPath("Dengb.ttf", cbuffer, MAX_PATH);
	mAxisRenderer.Initialize(mFramework->GetGDI());
	
	mBatch3D.Initialize(mFramework->GetGDI(), ConstantData::GetInstance().GetPCShaders(), 100, 100);
	mCube.SetPositionAndSize(-1, -1, -1, 2, 2, 2);

	mFont.Initialize(mFramework->GetGDI(), cbuffer, 16);
    mFont_s.Initialize(mFramework->GetGDI(), cbuffer, 12);
	mFont_b.Initialize(mFramework->GetGDI(), cbuffer, 20);
	mUITextRenderer.Initialize(mFramework->GetGDI(), &mFont, 1000);
    mTextRenderer_s.Initialize(mFramework->GetGDI(), &mFont_s, 1000);
	mTextRenderer_b.Initialize(mFramework->GetGDI(), &mFont_b, 200);
	mFramework->GetUIBatches().SetTextRenderer(FontSize::Default, &mUITextRenderer);

	Label * label = new Label();
	label->SetOnRemoveFromContainerLisener(Deleter);
	label->SetId(0);
	label->SetText(L"Direct3D11");
	label->SetPositionAndSize(200, 200, 100, 20);
	label->SetZ(0.4f);
	GetRootContainer().AddChild(label);

	label->GetClickHelper().AddOnClickListener([this, label](const MousePoint & ms, int mouseButton) {
		AsyncTask::NewTask(mFramework->GetTheard(), [this](AsyncTask * asyn) {
			MessageBox(NULL, L"YOU CLICK Label!!",L"Exe",0);
			asyn->Finish(0, 0);
		});
		label->GetTransform().AddTranslationAction(Action::Make(Point(200, 2, 0), 2.0, false, LinearInterpolator::GetInterpolator()));
	});
	Button * nextButton = new Button(2, L"X Game Render Framework\nClick here switch next scene.");
	GetRootContainer().AddChild(nextButton);
	nextButton->SetOnRemoveFromContainerLisener(Deleter);
	nextButton->SetPositionAndSize(0, 240, 230, 40);
	nextButton->SetZ(0.06f);
	nextButton->SetBorderSize(1.f);
	nextButton->GetClickHelper().AddOnClickListener([this](const MousePoint & ms, int mouseButton) {
		this->GetFramework()->SwitchScene(new SecondScene());
	});

	auto button = new Button(1, L"Button");
	button->SetOnRemoveFromContainerLisener(Deleter);
	GetRootContainer().AddChild(button);

	button->SetPositionAndSize(10,100,60,40);
	button->SetBorderSize(2);
	button->SetZ(0.07f);
	button->GetClickHelper().AddOnClickListener([this](const MousePoint & ms, int mouseButton) {
		if (mFramework->GetGDI()->GetDisplayMode() == DisplayMode::Borderless)
				mFramework->GetGDI()->SetDisplayMode(DisplayMode::Windowed, 0, 0, 600, 400, false);
			else if(mFramework->GetGDI()->GetDisplayMode() == DisplayMode::FullScreen)
				mFramework->GetGDI()->SetDisplayMode(DisplayMode::Borderless ,0 ,0 , 1920, 1080, true);
			else
				mFramework->GetGDI()->SetDisplayMode(DisplayMode::FullScreen, 0, 0, 1920, 1080, true);
	});
	
	EditText * edit1 = new EditText(10);
	EditText * edit2 = new EditText(11);
	edit1->SetOnRemoveFromContainerLisener(Deleter);
	edit2->SetOnRemoveFromContainerLisener(Deleter);

	edit1->SetPositionAndSize(300, 20, 200, 40);
	edit1->SetBorderSize(1);
	edit1->SetZ(0.08f);
	
	edit2->SetPositionAndSize(140, 20, 120, 40);
	edit2->SetBorderSize(2);
	edit2->SetZ(0.08f);

	GetRootContainer().AddChild(edit1);
	GetRootContainer().AddChild(edit2);

	//mFramework->GetInputManager()->GetCursor()->SetStaticTexture(mFramework->GetGDI(), GetFilePath(L"cursor.png", buffer, 100));

	mCamera.SetPos(XMFLOAT3(1,1,-10));

	//resource loading
	auto res = std::vector<ResourceInfo>();
	res.push_back(ResourceInfo(L"_normal.png", L"normal"));
	res.push_back(ResourceInfo(L"_hover.png", L"hover"));
	res.push_back(ResourceInfo(L"_activate.png", L"activate"));

	mTextureResourceManager.LoadResourceAsync(mFramework->GetGDI(), res, mFramework->GetTheard(), [this](std::vector<ResourceInfo> ress, int success) {
		auto t1 = Texture(*mTextureResourceManager.GetResourceByAlias(L"normal"));
		auto t2 = Texture(*mTextureResourceManager.GetResourceByAlias(L"hover"));
		auto t3 = Texture(*mTextureResourceManager.GetResourceByAlias(L"activate"));
		t1.Set9PathBorderSize(3);
		t2.Set9PathBorderSize(3);
		t3.Set9PathBorderSize(3);
		static_cast<Control *>(GetRootContainer().GetActorById(1))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));
		static_cast<Control *>(GetRootContainer().GetActorById(2))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));

		t1.Set9PathBorderSize(3.5);
		t2.Set9PathBorderSize(3.5);
		t3.Set9PathBorderSize(3.5);
		static_cast<Control *>(GetRootContainer().GetActorById(10))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));
		static_cast<Control *>(GetRootContainer().GetActorById(11))->SetSkin(Skin::CreateFromTextures(&t1, &t2, &t3));
	});
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

	mBatch3D.Shutdown();
	mTextureResourceManager.ReleaseAllResource();
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

void FirstScene::Update(float deltaTime)
{
    mCamera.Update();
    mCamera2D.Update();
	auto ip = GetFramework()->GetInputManager();
	if (ip->IskeyDowm(DIK_ESCAPE))
		GetFramework()->Exit(0);
}

void FirstScene::OnSize(int ClientX, int ClientY)
{
	mCamera2D.UpdateProject(ClientX, ClientY);
	mCamera.UpdateProject(ClientX, ClientY);
}

