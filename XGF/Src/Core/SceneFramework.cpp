#include "../../Include/SceneFramework.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Scene.hpp"
void SceneFramework::ISwitchScene(Scene * scene)
{
	scene->SetFramework(this);
	mScene->OnSwitchOut();
	scene->OnCreate();
	scene->OnSwitchIn();
	mScene->OnDestory();
	delete mScene;
	mScene = scene;
	mScene->OnSize(mGDI->GetWidth(),mGDI->GetHeight());
}
void SceneFramework::SwitchScene(Scene * scene)
{
	mTheard->PostEvent(EVENT_SWITCHSCENE,0,scene,0);
}
void SceneFramework::AddScene(Scene * scene)
{
	scene->SetFramework(this);
	scene->OnCreate();
	scene->OnSwitchIn();
	mScene = scene;
}

void SceneFramework::RenderScene()
{
	mScene->Render(mDeltaTime);
}

void SceneFramework::Clear(Color & c)
{
	mGDI->Clear(c);
}

SceneFramework::SceneFramework()
{
}

SceneFramework::~SceneFramework()
{
	delete mScene;
}

void SceneFramework::OnSize(int ClientX, int ClientY)
{
	if (mScene != nullptr)
		mScene->OnSize(ClientX, ClientY);
}

void SceneFramework::OnActivate(bool isActivate)
{
	if (mScene != nullptr)
		mScene->OnActivate(isActivate);
}

void SceneFramework::Render()
{
	if (mScene != nullptr)
		mScene->Render(mDeltaTime);
	mGDI->Present(mIsVsync);
}

void SceneFramework::Update(float time)
{
	if (mScene != nullptr)
		mScene->Updata(time);
}

void SceneFramework::OnMessage(const Event & ev)
{
	switch (ev.Message)
	{
	case EVENT_SWITCHSCENE:
			ISwitchScene((Scene *)ev.Content.y.address);
		break;
	default:
		break;
	}
}
