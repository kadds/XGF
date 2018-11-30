#include "../../Include/XGFramework.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Logger.hpp"
#include <DirectXMath.h>
#include "../../Include/Batch.hpp"
#include "../../Include/DebugInscriber.hpp"
#include "../../Include/AsyncTask.hpp"
#include "../../Include/ConstantData.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/ScreenGrab.h"
namespace XGF
{
	bool XGFramework::_Update(float time)
	{
		if (mThread->HandleMessage()) return true;
		mInputManager.Tick(time);
		if (mScene != nullptr)
			mScene->_Update(time);
		return false;
	}

	void XGFramework::_Loop2()
	{
		for (;;)
		{
			mDeltaTime = mMainTimer.Tick();
			if (_Update(mDeltaTime))
				return;
			DebugInscriber_Begin(mDeltaTime);
			if (mScene != nullptr)
			{
				mScene->_Render(mDeltaTime);//ÎÞ¶¯»­
			}
			mInputManager.Draw();
			mGDI->Present(mIsVSync);
			DebugInscriber_End();
		}
	}
	void XGFramework::_OnCreate(GDI *gdi, Asyn* asyn)
	{
		CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
		mGDI = gdi;
		mThread = asyn;
		asyn->SetCallBackFunc(std::bind(&EventDispatcher::Dispatch, &mFrameWorkEventDispatcher, std::placeholders::_1));
		mFrameWorkEventDispatcher.InsertAllEventListener(std::bind(&XGFramework::_OnMessage, this, std::placeholders::_1));
		mGDI->Create();
		ConstantData::GetInstance().Initialize(gdi);
		mUiBatches.Initialize(gdi);
		mInputManager.Initialize(gdi, gdi->GetInstance(), gdi->GetTopHwnd(), mThread);
	}

	void XGFramework::_OnDestroy()
	{
		if (mScene != nullptr)
		{
			mScene->_OnDestroy();
		}
		mInputManager.Shutdown();
		mUiBatches.Shutdown();
		ConstantData::GetInstance().Shutdown();
		mGDI->Destroy();

	}

	void XGFramework::_OnActivate(bool isActivate)
	{
		mGDI->CheckFullScreenForce(isActivate);
		mInputManager.OnActivate(isActivate);
	}

	void XGFramework::_OnSize(int ClientX, int ClientY)
	{
		if (ClientX <= 0) ClientX = 1;
		if (ClientY <= 0) ClientY = 1;
		mGDI->SizeChanged(ClientX, ClientY);
		mInputManager.UpdateCameraMatrix(ClientX, ClientY);
		Batch::SetClientSize({ ClientX, ClientY });
	}

	void XGFramework::_OnMessage(const Event& ev)
	{
		if (ev.mEventType == EventGroupType::System)
			switch (ev.GetSystemEventId())
			{
			case SystemEventId::Size:
				_OnSize(ev.GetDataInt(0), ev.GetDataInt(1));
				break;
			case SystemEventId::Activate:
				_OnActivate(ev.GetData<bool>(0));
				break;
			case SystemEventId::Close:
				_OnClose();
				break;
			case SystemEventId::AsynFinish:
				AsyncTask::DoFinishTaskEvent(ev);
				break;
			case SystemEventId::AsynReport:
				AsyncTask::DoReportTaskEvent(ev);
				break;
			case SystemEventId::SwitchScene:
				ISwitchScene(ev.GetDataSmartPointer<Scene>(0));
				break;
			default:
				break;
			}
		else if (ev.mEventType == EventGroupType::KeyBoard)
			_OnKeyBoardMessage(ev);
		else if (ev.mEventType == EventGroupType::Mouse)
			_OnMouseMessage(ev);
		mEventDispatcher.Dispatch(ev);
	}

	void XGFramework::_OnMouseMessage(const Event & ev)
	{
		if (ev.GetMouseEventId() == MouseEventId::MouseDown)
		{
			mInputManager.SetFocus(nullptr);
		}
		if (mOnInputListener != nullptr)
			mOnInputListener(*this, ev);
	}

	void XGFramework::_OnKeyBoardMessage(const Event & ev)
	{
		if (mOnInputListener != nullptr)
			mOnInputListener(*this, ev);
	}

	void XGFramework::_OnClose()
	{
		if (mOnCloseListener != nullptr && mOnCloseListener(*this))
			Exit(0);
	}
	void XGFramework::ISwitchScene(std::shared_ptr<Scene> scene)
	{
		XGF_Debug(Framework, "A scene to switch");
		scene->_OnCreate(this);
		scene->OnSize(mGDI->GetWidth(), mGDI->GetHeight());
		bool k = false;
		mScene->_OnDestroy();
		mScene = scene;
	}
	
	void XGFramework::AddScene(std::shared_ptr<Scene> scene)
	{
		scene->_OnCreate(this);
		mScene = scene;
	}
	void XGFramework::RenderScene()
	{
		mScene->_Render(mDeltaTime);
	}

	LRESULT XGFramework::OnInputMessage(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return mInputManager.ProcessInputMessage(msg, wParam, lParam);
	}

	InputManager& XGFramework::GetInputManager()
	{
		return mInputManager;
	}

	void XGFramework::SetOnCloseListener(std::function<bool(XGFramework&)> f)
	{
		mOnCloseListener = f;
	}

	void XGFramework::SetOnInputListener(std::function<bool(XGFramework&, const Event& ev)> f)
	{
		mOnInputListener = f;
	}

	UIBatches& XGFramework::GetUIBatches()
	{
		return mUiBatches;
	}

	EventDispatcher& XGFramework::GetEventDispatcher()
	{
		return mEventDispatcher;
	}

	void XGFramework::SwitchScene(std::shared_ptr<Scene> scene)
	{
		mThread->PostEvent(SystemEventId::SwitchScene, { scene });
	}
	void XGFramework::Clear(float color[]) const
	{
		mGDI->Clear(color);
	}

	void XGFramework::Clear(Color & color) const
	{
		mGDI->Clear(color);
	}

	void XGFramework::ClearDepthStencilBuffer() const
	{
		mGDI->ClearDepthStencilBuffer();
	}

	void XGFramework::Present(bool isVSync) const
	{
		mGDI->Present(isVSync);
	}

	void XGFramework::OpenVSync()
	{
		mIsVSync = true;
	}

	void XGFramework::CloseVSync()
	{
		mIsVSync = false;
	}

	HWND XGFramework::GetTopHwnd() const
	{
		return mGDI->GetTopHwnd();
	}

	HINSTANCE XGFramework::GetInstance() const
	{
		return mGDI->GetInstance();
	}

	Asyn &XGFramework::GetThread() const
	{
		return *mThread;
	}

	GDI & XGFramework::GetGDI() const
	{
		return *mGDI;
	}

	int XGFramework::GetWindowsWidth() const
	{
		return mGDI->GetWidth();
	}

	int XGFramework::GetWindowsHeight() const
	{
		return mGDI->GetHeight();
	}

	void XGFramework::Exit(int code)
	{
		PostMessage(mGDI->GetTopHwnd(), WM_CLOSE, 1, code);
		mThread->PostExitEvent();
	}


	XGFramework::XGFramework() : mGDI(nullptr), mDeltaTime(0), mThread(nullptr), mIsVSync(false),
	                             mOnCloseListener(nullptr),
	                             mOnInputListener(nullptr)
	{
		EventPool::Initialize(100);
	}

	XGFramework::~XGFramework()
	{
		EventPool::Shutdown();
	}
}
