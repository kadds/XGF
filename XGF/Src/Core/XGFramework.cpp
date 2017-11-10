#include "../../Include/XGFramework.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Log.hpp"
#include <DirectXMath.h>
#include "../../Include/Batch.hpp"
#include "../../Include/DebugInscriber.hpp"
#include "../../Include/AsyncTask.hpp"
#include "../../Include/ConstantData.hpp"
bool XGFramework::_Update(float time)
{
	if (mTheard->HandleMessage()) return true;
	mInputManager.Tick(time);
	Update(time);
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
		Render();
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
		Render();
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
	OnCreate();
}

void XGFramework::_OnDestory()
{
	OnDestory();
	mInputManager.Shutdown();
	ConstantData::GetInstance().Shutdown();
	mGDI->Destory();
	
}



void XGFramework::_OnActivate(bool isActivate)
{
	mInputManager.OnActivate(isActivate);
	OnActivate(isActivate);
}

void XGFramework::_OnSize(int ClientX, int ClientY)
{
	if (ClientX <= 0) ClientX = 1;
	if (ClientY <= 0) ClientY = 1;
	mGDI->SizeChanged(ClientX, ClientY);
	mInputManager.UpdateCameraMatrix(ClientX, ClientY);
	Batch::SetClientSize({ ClientX, ClientY });
	OnSize(ClientX, ClientY);
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
		mInputManager.OnMouseMove(mp.x, mp.y);
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
	if (OnClose())
		Exit(0);
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

XGFramework::XGFramework()
{
}

XGFramework::~XGFramework()
{
}
