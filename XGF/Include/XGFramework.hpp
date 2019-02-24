#pragma once
#include "Defines.hpp"
#include "Timer.hpp"
#include "InputManager.hpp"
#include "Asyn.hpp"
#include "EventDispatcher.hpp"

namespace XGF
{
	class Scene;
	class Context;

	class XGFramework
	{
	public:
		XGFramework();
		~XGFramework();
		void _Update(float deltaTime);
		void _UpdateWithInterpolation(float percent);

		void _Loop2();

		void _OnCreate();

		void _OnDestroy();

		void _OnActivate(bool isActivate);

		void _OnSize(int ClientX, int ClientY);

		void _OnMessage(const Event& ev);

		void _OnMouseMessage(const Event &ev);
		void _OnKeyBoardMessage(const Event &ev);

		void _OnClose();

		void OpenVSync();
		void CloseVSync();
		Asyn & GetThread() const;

		void Exit(int code);

		void SwitchScene(std::shared_ptr<Scene> scene);
		void AddScene(std::shared_ptr<Scene> scene);
		void RenderScene();
		LRESULT OnInputMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		InputManager& GetInputManager();

		void SetOnCloseListener(std::function<bool(XGFramework&)> f);
		void SetOnInputListener(std::function<bool(XGFramework&, const Event& ev)> f);;
		EventDispatcher& GetEventDispatcher();
		void SetLogicalFrameRate(int rate);
		struct AutoClose
		{
			bool operator()(XGFramework &) const
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
