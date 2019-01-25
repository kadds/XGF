#include "../../Include/XGFramework.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Logger.hpp"
#include <DirectXMath.h>
#include "../../Include/AsyncTask.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/ScreenGrab.h"
#include "../../Include/Context.hpp"
#include "../../Include/Renderer.hpp"

namespace XGF
{
	void XGFramework::_Loop2()
	{
		auto & context = Context::Current();
		for (;;)
		{
			mScheduler.DoSchedule();
			if (mThread->HandleMessage()) return;
		}
	}
	void XGFramework::_OnCreate()
	{
		CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
		auto & context = Context::Current();
		mThread = &context.QueryGameThread();
		context.QueryGameThread().SetCallBackFunc(std::bind(&EventDispatcher::Dispatch, &mFrameWorkEventDispatcher, std::placeholders::_1));
		mFrameWorkEventDispatcher.InsertAllEventListener(std::bind(&XGFramework::_OnMessage, this, std::placeholders::_1));
		mInputManager.Initialize();
		context.QueryGameThread().PostEvent(SystemEventId::LogicalFrame, {});
		mUpdateFixedTime = 0.0166666666f;
		mScheduler.AddSchedule(ScheduleInfo(mUpdateFixedTime, std::bind(&XGFramework::_Update, this, std::placeholders::_1), "Update"));
	}

	void XGFramework::_OnDestroy()
	{
		if (mScene != nullptr)
		{
			mScene->_OnDestroy();
		}
		mInputManager.Shutdown();
		mScheduler.RemoveSchedule("Update");
	}

	void XGFramework::_OnActivate(bool isActivate)
	{
		mInputManager.OnActivate(isActivate);
	}

	void XGFramework::_OnSize(int ClientX, int ClientY)
	{
		if (ClientX <= 0) ClientX = 1;
		if (ClientY <= 0) ClientY = 1;
		mInputManager.UpdateCameraMatrix(ClientX, ClientY);
	}

	void XGFramework::_OnMessage(const Event& ev)
	{
		if (ev.mEventType == EventGroupType::System)
		{
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
			case SystemEventId::LogicalFrame:
				{
					if(mUpdateFixedTime > 0.f)
						_UpdateWithInterpolation(mScheduler.GetDeltaTimeFromLastSchedule() / mUpdateFixedTime);
					if (mScene != nullptr)
					{
						mScene->_Render();//无动画
					}
					mInputManager.Draw();
					auto & context = Context::Current();
					context.QueryRenderThread().PostEvent(SystemEventId::RenderFrame, {});
				}
				break;
			default:
				break;
			}
		}
		else if (ev.mEventType == EventGroupType::KeyBoard)
			_OnKeyBoardMessage(ev);
		else if (ev.mEventType == EventGroupType::Mouse)
			_OnMouseMessage(ev);
		mEventDispatcher.Dispatch(ev);
	}

	void XGFramework::_OnMouseMessage(const Event & ev)
	{
		if(ev.GetMouseEventId() == MouseEventId::MouseMove)
			mInputManager.OnMouseMove(static_cast<float>(ev.GetData<int>(0)), static_cast<float>(ev.GetData<int>(1)));
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
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		scene->OnSize(gdi.GetWidth(), gdi.GetHeight());
		bool k = false;
		mScene->_OnDestroy();
		mScene = scene;
	}
	
	void XGFramework::SwitchScene(std::shared_ptr<Scene> scene)
	{
		mThread->PostEvent(SystemEventId::SwitchScene, { scene });
	}

	void XGFramework::AddScene(std::shared_ptr<Scene> scene)
	{
		scene->_OnCreate(this);
		mScene = scene;
	}
	void XGFramework::RenderScene()
	{
		mScene->_Render();
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

	EventDispatcher& XGFramework::GetEventDispatcher()
	{
		return mEventDispatcher;
	}

	void XGFramework::SetLogicalFrameRate(int rate)
	{
		mScheduler.RemoveSchedule("Update");
		if (rate == 0)
			mUpdateFixedTime = 0.f;
		else
			mUpdateFixedTime = 1.f / rate;
		mScheduler.AddSchedule(ScheduleInfo(mUpdateFixedTime, std::bind(&XGFramework::_Update, this, std::placeholders::_1), "Update"));
	}

	void XGFramework::SetInfoFrameCost(float frameCost)
	{
		mInfoFrameCost = frameCost;
	}

	void XGFramework::OpenVSync()
	{
		mIsVSync = true;
	}

	void XGFramework::CloseVSync()
	{
		mIsVSync = false;
	}


	Asyn &XGFramework::GetThread() const
	{
		return *mThread;
	}

	void XGFramework::Exit(int code)
	{
		// 设置退出码
		PostMessage(Context::Current().QueryGraphicsDeviceInterface().GetTopHwnd(), WM_CLOSE, 1, code);
		mThread->PostExitEvent();
		Context::Current().QueryRenderThread().PostExitEvent();
	}


	XGFramework::XGFramework() : mThread(nullptr), mIsVSync(false),
	                             mOnCloseListener(nullptr),
	                             mOnInputListener(nullptr), mInfoFrameCost(1.f / 30)
	{
	}

	XGFramework::~XGFramework()
	{
	}

	void XGFramework::_Update(float deltaTime)
	{
		if (deltaTime >= mInfoFrameCost)
		{
			XGF_Info(Framework, "logical frame delta time: ", deltaTime * 1000, "ms");
		}
		mInputManager.Tick(deltaTime);
		if (mScene != nullptr)
			mScene->_Update(deltaTime);
	}
	void XGFramework::_UpdateWithInterpolation(float percent)
	{
		_Update(percent * mUpdateFixedTime);
	}
}
