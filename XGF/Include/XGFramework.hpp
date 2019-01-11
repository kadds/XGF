#pragma once
#include "Timer.hpp"
#include "Defines.hpp"
#include "InputManager.hpp"
#include "Asyn.hpp"
#include "EventDispatcher.hpp"
#include "UIBatches.hpp"

namespace XGF
{
	class Scene;
	class Context;
	/*
	����Ӧ�ó�����
	*/
	class XGFramework
	{
	public:
		XGFramework();
		~XGFramework();
		bool _Update(float time);
		//Application��ܵ���
		void _Loop2();
		//Application��ܵ���
		void _OnCreate();
		//Application��ܵ���
		void _OnDestroy();

		void _OnActivate(bool isActivate);

		void _OnSize(int ClientX, int ClientY);
		//Application��ܵ���
		void _OnMessage(const Event& ev);
		//��ܵ��ã��������Ȥ����Ϣ��ϵͳ��Ϣ��������
		//Application��ܵ���
		void _OnMouseMessage(const Event &ev);
		void _OnKeyBoardMessage(const Event &ev);
		//Application��ܵ���
		void _OnClose();

		void OpenVSync();
		void CloseVSync();
		Asyn & GetThread() const;
		//�˳�ѭ��
		void Exit(int code);
		//�л�Scene��ע�⣬�ú���ֻ������Ϣ�������������Ϣ����һ֡�Ż��л���ʵ���л�������ISwitchScene��
		void SwitchScene(std::shared_ptr<Scene> scene);
		void AddScene(std::shared_ptr<Scene> scene);
		//��ȾScene
		void RenderScene();
		//Application��ܵ���
		LRESULT OnInputMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		InputManager& GetInputManager();

		void SetOnCloseListener(std::function<bool(XGFramework&)> f);
		void SetOnInputListener(std::function<bool(XGFramework&, const Event& ev)> f);;
		UIBatches& GetUIBatches();
		EventDispatcher& GetEventDispatcher();

		struct AutoClose
		{
			bool operator()(XGFramework &)
			{
				return true;
			}
		};
	protected:
		std::shared_ptr<Scene> mScene;
		void ISwitchScene(std::shared_ptr<Scene> scene);
		Timer mMainTimer;
		float mDeltaTime;
		InputManager mInputManager;

		Asyn * mThread;
		bool mIsVSync;
		UIBatches mUiBatches;
		std::function<bool(XGFramework &)> mOnCloseListener;
		std::function<bool(XGFramework &, const Event &ev)> mOnInputListener;

	private:
		EventDispatcher mEventDispatcher;

		EventDispatcher mFrameWorkEventDispatcher;
	};

};
