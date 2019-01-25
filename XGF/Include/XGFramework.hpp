#pragma once
#include "Timer.hpp"
#include "Defines.hpp"
#include "InputManager.hpp"
#include "Asyn.hpp"
#include "EventDispatcher.hpp"

namespace XGF
{
	class Scene;
	class Context;
	/*
	这是应用程序框架
	*/
	class XGFramework
	{
	public:
		XGFramework();
		~XGFramework();
		void _Update(float deltaTime);
		void _UpdateWithInterpolation(float percent);
		//Application框架调用
		void _Loop2();
		//Application框架调用
		void _OnCreate();
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

		void OpenVSync();
		void CloseVSync();
		Asyn & GetThread() const;
		//退出循环
		void Exit(int code);
		//切换Scene。注意，该函数只是在消息队列中添加了消息，下一帧才会切换，实际切换代码在ISwitchScene中
		void SwitchScene(std::shared_ptr<Scene> scene);
		void AddScene(std::shared_ptr<Scene> scene);
		//渲染Scene
		void RenderScene();
		//Application框架调用
		LRESULT OnInputMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		InputManager& GetInputManager();

		void SetOnCloseListener(std::function<bool(XGFramework&)> f);
		void SetOnInputListener(std::function<bool(XGFramework&, const Event& ev)> f);;
		EventDispatcher& GetEventDispatcher();
		void SetLogicalFrameRate(int rate);
		struct AutoClose
		{
			bool operator()(XGFramework &)
			{
				return true;
			}
		};

		void SetInfoFrameCost(float frameCost);
	protected:
		std::shared_ptr<Scene> mScene;
		void ISwitchScene(std::shared_ptr<Scene> scene);
		Scheduler mScheduler;
		InputManager mInputManager;

		Asyn * mThread;
		bool mIsVSync;
		std::function<bool(XGFramework &)> mOnCloseListener;
		std::function<bool(XGFramework &, const Event &ev)> mOnInputListener;
		float mUpdateFixedTime;
	private:
		EventDispatcher mEventDispatcher;

		EventDispatcher mFrameWorkEventDispatcher;
		float mInfoFrameCost;
	};

};
