#include "../../Include/XGFramework.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Log.hpp"
#include <DirectXMath.h>
#include "../../Include/Batch.hpp"
#include "../../Include/DebugInscriber.hpp"
#include "../../Include/AsyncTask.hpp"
#include "../../Include/ConstantData.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/ScreenGrab.h"
bool XGFramework::_Update(float time)
{
	if (mTheard->HandleMessage()) return true;
	mInputManager.Tick(time);
	if (mScene != nullptr)
		mScene->Updata(time);
	if (mSceneAnimation != nullptr)
	{
		mSceneAnimation->Updata(time);
		if (mSceneAnimation->IsEnd())
		{
			mSceneAnimation = nullptr;
		}
	}
	if (mLastScene != nullptr)
	{
		mLastScene->Updata(time);
		if (mLastSceneAnimation != nullptr)
		{
			mLastSceneAnimation->Updata(time);
			if (mLastSceneAnimation->IsEnd())
			{
				mLastScene->OnDestory();
				if (mSceneDeleter != nullptr)
				{
					mSceneDeleter(mLastScene);
					mLastScene = nullptr;
				}
			}
		}
	}
	
	return false;
}
void XGFramework::_Loop()
{
    //_Update(1000.0f / 50);
    for (;;)
	{
		mDeltaTime = mainTimer.Tick();
		long tk = mainTimer.GetTimeCount();
		int loops = 0;
		float nextFrameTime = mainTimer.GetTime(tk);
		while (mainTimer.GetTime(mainTimer.GetTimeCount())> nextFrameTime
			&& loops < 10)
		{
			if (_Update(1000.0f / 50))
				return ;
			nextFrameTime += 1000.0f / 50;
			loops++;
			//DebugOut(L"update ");
		}
		DebugInscriber_Begin(mDeltaTime);
		if (mScene != nullptr)
			mScene->Render(mDeltaTime);
		if (mLastScene != nullptr)
		{
			mLastScene->Render(mDeltaTime);
		}
		mInputManager.Draw();
		mGDI->Present(mIsVsync);
		DebugInscriber_End();
		//DebugOut(L"\nRender\n");
	}
}
void XGFramework::_Loop2()
{
    for (;;)
	{
		mDeltaTime = mainTimer.Tick();
			if (_Update(mDeltaTime))
				return;
		//DebugOut(L"update ");
		DebugInscriber_Begin(mDeltaTime);
		if (mLastScene != nullptr)
		{
			mGDI->PushRTTLayer(&mRenderToTexture);
			mGDI->PushRTTLayer(&mLastRenderToTexture);
			mGDI->DrawRTT();
			mLastScene->Render(mDeltaTime);
			mGDI->PopRTTLayer();
			mGDI->DrawRTT();
			if (mScene != nullptr)
				mScene->Render(mDeltaTime);
			mGDI->PopRTTLayer();
			mGDI->DrawRTT();
			DrawSceneAnimation();
		}
		else if (mScene != nullptr)
		{
			if (mSceneAnimation != nullptr)
			{
				mGDI->PushRTTLayer(&mRenderToTexture);
				mGDI->DrawRTT();
				mScene->Render(mDeltaTime);
				mGDI->PopRTTLayer();
				mGDI->DrawRTT();
				DrawSceneAnimation();
			}
			else
			{
				mScene->Render(mDeltaTime);
			}
		}
		mInputManager.Draw();
		mGDI->Present(mIsVsync);
		DebugInscriber_End();
		//DebugOut(L"\nRender\n");
	}
}
void XGFramework::_OnCreate(GDI *gdi, Asyn* asyn)
{
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	mGDI = gdi;
	mTheard = asyn;
	asyn->SetCallBackFunc(std::bind(&XGFramework::_OnMessage,this, std::placeholders::_1));
	mGDI->Create();
	ConstantData::GetInstance().Initialize(gdi);
	mInputManager.Initialize(gdi, gdi->GetInstance(), gdi->GetTopHwnd(),mTheard);
	mSceneBatch.Initialize(gdi, ConstantData::GetInstance().GetPCTShader(), 24, 24);
	mSceneBatch.SetBlend(true);
	mSceneBatch.SetZBufferRender(false);
	mRenderToTexture.Initialize(mGDI, mGDI->GetWidth(), mGDI->GetHeight());
	mLastRenderToTexture.Initialize(mGDI, mGDI->GetWidth(), mGDI->GetHeight());
}

void XGFramework::_OnDestory()
{
	if (mScene != nullptr)
	{
		mScene->OnDestory();
		if (mSceneDeleter != nullptr)
			mSceneDeleter(mScene);
		if (mLastScene != nullptr)
		{
			mLastScene->OnDestory();
			if (mSceneDeleter != nullptr)
				mSceneDeleter(mLastScene);
		}
			
	}
	mSceneBatch.Shutdown();
	mRenderToTexture.Shutdown();
	mLastRenderToTexture.Shutdown();
	mInputManager.Shutdown();
	ConstantData::GetInstance().Shutdown();
	mGDI->Destory();
	
}



void XGFramework::_OnActivate(bool isActivate)
{
	mGDI->CheckFullScreenForce(isActivate);
	mInputManager.OnActivate(isActivate);
	if (mScene != nullptr)
		mScene->OnActivate(isActivate);
	if(mLastScene != nullptr)
		mLastScene->OnActivate(isActivate);
}

void XGFramework::_OnSize(int ClientX, int ClientY)
{
	if (ClientX <= 0) ClientX = 1;
	if (ClientY <= 0) ClientY = 1;
	mGDI->SizeChanged(ClientX, ClientY);
	mInputManager.UpdateCameraMatrix(ClientX, ClientY);
	mRenderCamera.UpdataProject(ClientX, ClientY);
	Batch::SetClientSize({ ClientX, ClientY });
	if (mScene != nullptr)
		mScene->OnSize(ClientX, ClientY);
	if (mLastScene != nullptr)
		mLastScene->OnSize(ClientX, ClientY);
	mLastRenderRectangle.SetPositionAndSize(0.f, 0.f, static_cast<float>(ClientX), static_cast<float>(ClientY));
	mLastRenderRectangle.SetZ(0.f);
	mRenderRectangle.SetPositionAndSize(0, 0, static_cast<float>(ClientX), static_cast<float>(ClientY));
	mRenderRectangle.SetZ(0.f);
	mRenderToTexture.Shutdown();
	mLastRenderToTexture.Shutdown();
	mRenderToTexture.Initialize(mGDI, ClientX, ClientY);
	mLastRenderToTexture.Initialize(mGDI, ClientX, ClientY);
}

void XGFramework::_OnMessage(const Event& ev)
{
	switch (ev.Message)
	{
	case EVENT_ONSIZE:
		_OnSize(ev.Content.x.num, ev.Content.y.num);
		break;
	case EVENT_ONACTIVATE:
		_OnActivate(ev.Content.x.num == 0 ? false : true);
		break;
	case EVENT_ONCLOSE:
		_OnClose();
		break;
	case EVENT_ONASYNCTASKFINISH:
		AsyncTask::DoFinishTaskEvent(ev);
		break;
	case EVENT_ONASYNCTASKREPORT:
		AsyncTask::DoReportTaskEvent(ev);
		break;
	case EVENT_SWITCHSCENE:
		ISwitchScene((Scene *)ev.Content.y.address);
		break;
	default:
		if (!_OnInput(ev))
			OnMessage(ev);
		break;
	}
}

bool XGFramework::_OnInput(const Event & ev)
{
	MousePoint mp;
	switch (ev.Message)
	{
	case EVENT_ONMOUSEDOWM:
		mp.x = ev.Content.x.num;
		mp.y = ev.Content.y.num;
		GetInputManager()->StartForForce();
		for each (auto var in mInputs)
		{
			var->OnMouseDowm(mp, ev.Content.z.num);
		}
		GetInputManager()->StopForForce();
		break;
	case EVENT_ONMOUSEMOVE:
		mp.x = ev.Content.x.num;
		mp.y = ev.Content.y.num;
		mInputManager.OnMouseMove(static_cast<float>(mp.x), static_cast<float>(mp.y));
		for each (auto var in mInputs)
		{
			var->OnMouseMove(mp, ev.Content.z.num);
		}
		break;
	case EVENT_ONMOUSEUP:
		mp.x = ev.Content.x.num;
		mp.y = ev.Content.y.num;
		for each (auto var in mInputs)
		{
			var->OnMouseUp(mp, ev.Content.z.num);
		}
		break;
	case EVENT_ONKEYDOWM:
		for each (auto var in mInputs)
		{
			var->OnKeyDowm(ev.Content.x.num);
		}
		break;
	case EVENT_ONKEYUP:
		for each (auto var in mInputs)
		{
			var->OnKeyUp(ev.Content.x.num);
		}
		break;
	default:
		return false;
	}
	return true;
}

void XGFramework::_OnClose()
{
	if (mOnClose != nullptr && mOnClose())
		Exit(0);
}
void XGFramework::ISwitchScene(Scene * scene)
{
	scene->SetFramework(this);
	scene->OnCreate();
	SceneAnimation * sa = scene->OnSwitchIn();
	SceneAnimation * la = mScene->OnSwitchOut();
	mLastSceneAnimation = la;
	mSceneAnimation = sa;
	bool k = false;
	if (sa != nullptr)
	{
		sa->BeginAnimation();
		k = true;
	}
	if(la != nullptr)
	{
		la->BeginAnimation();
		k = true;
	}
	if(!k)
	{
		mScene->OnDestory();
		if (mSceneDeleter != nullptr)
			mSceneDeleter(mScene);
		mScene = scene;
	}
	else
	{
		mLastScene = mScene;
		mScene = scene;
	}
	mScene->OnSize(mGDI->GetWidth(), mGDI->GetHeight());
}
void XGFramework::DrawSceneAnimation()
{
	mGDI->Clear(Color(1.0f,1.0f,1.0f,1.f));
	WVPMatrix wvp;
	Color c;
	BindingBridge bbr;
	PolygonPleTextureBinder ppb(4);
	PolygonPleConstantColorBinder cc(Color(0.f,0.f,0.f,1.f),4);
	bbr.AddBinder(cc);
	bbr.AddBinder(ppb);
	ppb.SetPosition(0.f, 1.f, 0.f, 1.f);
	mRenderCamera.GetCameraMatrix(wvp);
	mSceneBatch.Begin(wvp);
	if (mLastSceneAnimation != nullptr)
	{
		mLastSceneAnimation->GetColor(c,0);
		cc.Set(0, 1, c);
		mLastRenderRectangle.Render(mSceneBatch, &mLastSceneAnimation->GetMatrix(), bbr, mLastRenderToTexture.GetShaderResourceView());
	}
	else
		mLastRenderRectangle.Render(mSceneBatch, nullptr, bbr, mLastRenderToTexture.GetShaderResourceView());
	if (mSceneAnimation != nullptr)
	{
		mSceneAnimation->GetColor(c, 0);
		cc.Set(0, 1, c);
		mRenderRectangle.Render(mSceneBatch, &mSceneAnimation->GetMatrix(), bbr, mRenderToTexture.GetShaderResourceView());
	}
	else
	{
		cc.Set(0, 1, Color(1.f,1.f,1.f,1.f));
		mRenderRectangle.Render(mSceneBatch, nullptr, bbr, mRenderToTexture.GetShaderResourceView());
	}
		
	mSceneBatch.End();
	
	
}
void XGFramework::AddScene(Scene * scene)
{
	scene->SetFramework(this);
	scene->OnCreate();
	SceneAnimation * sa = scene->OnSwitchIn();
	if (sa != nullptr)
	{
		mSceneAnimation = sa;
		sa->BeginAnimation();
	}
	mScene = scene;
}
void XGFramework::RenderScene()
{
	mScene->_Render(mDeltaTime);
}
void XGFramework::SwitchScene(Scene * scene)
{
	mTheard->PostEvent(EVENT_SWITCHSCENE, 0, scene, 0);
}
void XGFramework::Clear(float color[])
{
	mGDI->Clear(color);
}

void XGFramework::Clear(Color & color)
{
	mGDI->Clear(color);
}

void XGFramework::ClearDepthStencilBuffer()
{
	mGDI->ClearDepthStencilBuffer();
}

void XGFramework::Present(bool isVsync)
{
	mGDI->Present(isVsync);
}

HWND XGFramework::GetTopHwnd()
{
	return mGDI->GetTopHwnd();
}

HINSTANCE XGFramework::GetInstance()
{
	return mGDI->GetInstance();
}

GDI * XGFramework::GetGDI()
{
	return mGDI;
}

int XGFramework::GetWindowsWidth()
{
	return mGDI->GetWidth();
}

int XGFramework::GetWindowsHeight()
{
	return mGDI->GetHeight();
}

void XGFramework::Exit(int code)
{
	PostMessage(mGDI->GetTopHwnd(), WM_CLOSE, 1, code);
	mTheard->PostExitEvent();
}

void XGFramework::AddInputListener(InputListener * il)
{
	mInputs.push_back(il);
}

void XGFramework::RemoveInputListener(InputListener * il)
{
	mInputs.remove(il);
}

XGFramework::XGFramework():mSceneDeleter(nullptr),mOnClose(nullptr), mOnInput(nullptr)
{
}

XGFramework::~XGFramework()
{
}
