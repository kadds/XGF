#pragma once
#include "Timer.hpp"
#include "Defines.hpp"
#include "InputManager.hpp"
#include "Asyn.hpp"
#include "SceneAnimation.hpp"
#include "Batch.hpp"
#include "Shader.hpp"
#include "OrthoCamera.hpp"
#include "Rectangle.hpp"
#include "RenderToTexture.hpp"
#include "EventDispatcher.hpp"
#include "UIBatches.hpp"
namespace XGF
{
	class Scene;
	class GDI;
	/*
	这是应用程序框架
	*/
	class XGFramework
	{
	public:
		XGFramework();
		~XGFramework();
		bool _Update(float time);
		//Application框架调用
		void _Loop2();
		//Application框架调用
		void _OnCreate(GDI *gdi, Asyn * asyn);
		//Application框架调用
		void _OnDestroy();

		void _OnActivate(bool isActivate);

		void _OnSize(int ClientX, int ClientY);
		//Application框架调用
		void _OnMessage(const Event& ev);
		//框架调用，处理感兴趣的消息，系统消息均被过滤
		//Application框架调用
		void _OnMouseMessage(const Event &ev);
		void _OnKeyBoardMessage(const Event &ev);
		//Application框架调用
		void _OnClose();

		void Clear(float color[]);
		void Clear(SM::Color &color);
		void ClearDepthStencilBuffer();
		void Present(bool isVsync);

		void OpenVsync() { mIsVsync = true; }
		void CloseVsync() { mIsVsync = false; }
		HWND GetTopHwnd();
		HINSTANCE GetInstance();
		Asyn * GetTheard() { return mTheard; }
		GDI * GetGDI();

		int GetWindowsWidth();
		int GetWindowsHeight();
		//退出循环
		void Exit(int code);
		//切换Scene。注意，该函数只是在消息队列中添加了消息，下一帧才会切换，实际切换代码在ISwithScene中
		//可确保在Click事件中调用不会破坏迭代器
		void SwitchScene(std::shared_ptr<Scene> scene);
		void AddScene(std::shared_ptr<Scene> scene);
		//渲染Scene
		void RenderScene();
		//Application框架调用
		LRESULT OnInputMessage(UINT msg, WPARAM wParam, LPARAM lParam) { return mInputManager.ProcessInputMessage(msg, wParam, lParam); }

		InputManager * GetInputManager() { return &mInputManager; }

		void SetOnClose(std::function<bool()> f) { mOnClose = f; };
		void SetOnInput(std::function<bool(const Event &ev)> f) { mOnInput = f; };

		EventDispatcher & GetEventDispatcher() { return mEventDispatcher; }

		UIBatches & GetUIBatches() { return mUIBatches; };
	protected://来自外部初始化的变量
		GDI *mGDI;
	protected:
		std::shared_ptr<Scene> mScene;
		std::shared_ptr<Scene> mLastScene;
		void ISwitchScene(std::shared_ptr<Scene> scene);
		Timer mainTimer;
		float mDeltaTime;
		InputManager mInputManager;

		Asyn * mTheard;
		bool mIsVsync;

		std::function<bool()> mOnClose;
		std::function<bool(const Event &ev)> mOnInput;

		RenderToTexture mRenderToTexture, mLastRenderToTexture;
		SceneAnimation * mSceneAnimation, *mLastSceneAnimation;
		Batch mSceneBatch;
		OrthoCamera mRenderCamera;
		Shape::Rectangle mRenderRectangle, mLastRenderRectangle;

		UIBatches mUIBatches;
	private:
		EventDispatcher mEventDispatcher;

		EventDispatcher mFrameWorkEventDispatcher;
		void DrawSceneAnimation();
		DISALLOW_COPY_AND_ASSIGN(XGFramework);
	};

};
