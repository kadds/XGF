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
namespace XGF
{
	bool XGFramework::_Update(float time)
	{
		if (mTheard->HandleMessage()) return true;
		mInputManager.Tick(time);
		if (mScene != nullptr)
			mScene->_Update(time);
		if (mSceneAnimation != nullptr)
		{
			mSceneAnimation->Update(time);
			if (mSceneAnimation->IsEnd())
			{
				mSceneAnimation = nullptr;
			}
		}
		if (mLastScene != nullptr)
		{
			mLastScene->_Update(time);
			if (mLastSceneAnimation != nullptr)
			{
				mLastSceneAnimation->Update(time);
				if (mLastSceneAnimation->IsEnd())//动画执行完毕，结束
				{
					mLastScene->OnDestroy();
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

	void XGFramework::_Loop2()
	{
		for (;;)
		{
			mDeltaTime = mainTimer.Tick();
			if (_Update(mDeltaTime))
				return;
			DebugInscriber_Begin(mDeltaTime);
			WVPMatrix wvp;
			mRenderCamera.GetCameraMatrix(wvp);
			mUIBatches.Begin(wvp);
			if (mLastScene != nullptr)
			{
				mGDI->PushRTTLayer(&mRenderToTexture);
				mGDI->PushRTTLayer(&mLastRenderToTexture);
				mGDI->DrawRTT();
				mLastScene->_Render(mDeltaTime);
				mGDI->PopRTTLayer();
				mGDI->DrawRTT();
				if (mScene != nullptr)
					mScene->_Render(mDeltaTime);
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
					mScene->_Render(mDeltaTime);
					mGDI->PopRTTLayer();
					mGDI->DrawRTT();
					DrawSceneAnimation();
				}
				else
				{
					mScene->_Render(mDeltaTime);//无动画
				}
			}
			mUIBatches.End();
			mInputManager.Draw();
			mGDI->Present(mIsVsync);
			DebugInscriber_End();
		}
	}
	void XGFramework::_OnCreate(GDI *gdi, Asyn* asyn)
	{
		CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
		mGDI = gdi;
		mTheard = asyn;
		asyn->SetCallBackFunc(std::bind(&EventDispatcher::Dispatch, &mFrameWorkEventDispatcher, std::placeholders::_1));
		mFrameWorkEventDispatcher.InsertAllEventListener(std::bind(&XGFramework::_OnMessage, this, std::placeholders::_1));
		mGDI->Create();
		ConstantData::GetInstance().Initialize(gdi);
		mInputManager.Initialize(gdi, gdi->GetInstance(), gdi->GetTopHwnd(), mTheard);
		mSceneBatch.Initialize(gdi, ConstantData::GetInstance().GetPCTShaders(), 24, 24);
		mSceneBatch.GetShaderStage()->SetBlendState(BlendState::AddOneOneAdd);
		mSceneBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthDisable);
		mRenderToTexture.Initialize(mGDI, mGDI->GetWidth(), mGDI->GetHeight());
		mLastRenderToTexture.Initialize(mGDI, mGDI->GetWidth(), mGDI->GetHeight());
		mUIBatches.Initialize(gdi);
	}

	void XGFramework::_OnDestroy()
	{
		mUIBatches.Shutdown();
		if (mScene != nullptr)
		{
			mScene->_OnDestroy();
			if (mSceneDeleter != nullptr)
				mSceneDeleter(mScene);
			if (mLastScene != nullptr)
			{
				mLastScene->_OnDestroy();
				if (mSceneDeleter != nullptr)
					mSceneDeleter(mLastScene);
			}

		}
		mSceneBatch.Shutdown();
		mRenderToTexture.Shutdown();
		mLastRenderToTexture.Shutdown();
		mInputManager.Shutdown();
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
		mRenderCamera.UpdateProject(ClientX, ClientY);
		Batch::SetClientSize({ ClientX, ClientY });
		mLastRenderRectangle.SetPositionAndSize(0.f, 0.f, static_cast<float>(ClientX), static_cast<float>(ClientY));
		mLastRenderRectangle.SetZ(0.1f);
		mRenderRectangle.SetPositionAndSize(0, 0, static_cast<float>(ClientX), static_cast<float>(ClientY));
		mRenderRectangle.SetZ(0.01f);
		mRenderToTexture.Shutdown();
		mLastRenderToTexture.Shutdown();
		mRenderToTexture.Initialize(mGDI, ClientX, ClientY);
		mLastRenderToTexture.Initialize(mGDI, ClientX, ClientY);
	}

	void XGFramework::_OnMessage(const Event& ev)
	{
		if (ev.mEventType == EventGroup::System)
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
				ISwitchScene(ev.GetData<Scene *>(0));
				break;
			default:
				break;
			}
		else if (ev.mEventType == EventGroup::KeyBoard)
			_OnKeyBoardMessage(ev);
		else if (ev.mEventType == EventGroup::Mouse)
			_OnMouseMessage(ev);
		mEventDispatcher.Dispatch(ev);
	}

	void XGFramework::_OnMouseMessage(const Event & ev)
	{
		if (ev.GetMouseEventId() == MouseEventId::MouseDown)
		{
			mInputManager.SetFocus(nullptr);
		}
	}

	void XGFramework::_OnKeyBoardMessage(const Event & ev)
	{
		//Nothing to do
	}

	void XGFramework::_OnClose()
	{
		if (mOnClose != nullptr && mOnClose())
			Exit(0);
	}
	void XGFramework::ISwitchScene(Scene * scene)
	{
		XGF_ReportDebug0("A scene to switch");
		scene->_OnCreate(this);
		scene->OnSize(mGDI->GetWidth(), mGDI->GetHeight());
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
		if (la != nullptr)
		{
			la->BeginAnimation();
			k = true;
		}
		if (!k)
		{
			mScene->_OnDestroy();
			if (mSceneDeleter != nullptr)
				mSceneDeleter(mScene);
			mScene = scene;
		}
		else
		{
			mLastScene = mScene;
			mScene = scene;
		}
	}
	void XGFramework::DrawSceneAnimation()
	{
		mGDI->Clear(SM::Color(1.0f, 1.0f, 1.0f, 1.f));
		WVPMatrix wvp;
		SM::Color c;
		BindingBridge bbr;
		PolygonPleTextureBinder ppb(4);
		PolygonPleConstantColorBinder cc(SM::Color(0.f, 0.f, 0.f, 1.f), 4);
		bbr.AddBinder(cc);
		bbr.AddBinder(ppb);
		ppb.SetPosition(0.f, 1.f, 0.f, 1.f);
		mRenderCamera.GetCameraMatrix(wvp);
		mSceneBatch.GetShaderStage()->SetVSConstantBuffer(0, &wvp);
		mSceneBatch.Begin();
		if (mLastSceneAnimation != nullptr)
		{
			mLastRenderRectangle.mTransform.SetMatrix(mLastSceneAnimation->GetMatrix());
			mLastSceneAnimation->GetColor(c, 0);
			cc.Set(0, 1, c);
			mLastRenderRectangle.Render(mSceneBatch, bbr, mLastRenderToTexture.GetShaderResourceView());
		}
		else {
			mRenderRectangle.mTransform.SetMatrix(DirectX::XMMatrixIdentity());
			mLastRenderRectangle.Render(mSceneBatch, bbr, mLastRenderToTexture.GetShaderResourceView());
		}
		mSceneBatch.Flush();
		if (mSceneAnimation != nullptr)
		{
			mRenderRectangle.mTransform.SetMatrix(mSceneAnimation->GetMatrix());
			mSceneAnimation->GetColor(c, 0);
			cc.Set(0, 1, c);
			mRenderRectangle.Render(mSceneBatch, bbr, mRenderToTexture.GetShaderResourceView());
		}
		else
		{
			mRenderRectangle.mTransform.SetMatrix(DirectX::XMMatrixIdentity());
			cc.Set(0, 1, SM::Color(1.f, 1.f, 1.f, 1.f));
			mRenderRectangle.Render(mSceneBatch, bbr, mRenderToTexture.GetShaderResourceView());
		}

		mSceneBatch.End();


	}
	void XGFramework::AddScene(Scene * scene)
	{
		scene->_OnCreate(this);
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
		mTheard->PostEvent(SystemEventId::SwitchScene, { scene });
	}
	void XGFramework::Clear(float color[])
	{
		mGDI->Clear(color);
	}

	void XGFramework::Clear(SM::Color & color)
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


	XGFramework::XGFramework() :mSceneDeleter(nullptr), mOnClose(nullptr), mOnInput(nullptr)
	{
	}

	XGFramework::~XGFramework()
	{
	}
}
