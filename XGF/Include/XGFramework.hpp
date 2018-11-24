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
		void _OnCreate(GDI *gdi, Asyn * asyn);
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

		void Clear(float color[]) const;
		void Clear(SM::Color &color) const;
		void ClearDepthStencilBuffer() const;
		void Present(bool isVSync) const;

		void OpenVSync() { mIsVSync = true; }
		void CloseVSync() { mIsVSync = false; }
		HWND GetTopHwnd() const;
		HINSTANCE GetInstance() const;
		Asyn * GetThread() const { return mThread; }
		GDI * GetGDI() const;

		int GetWindowsWidth() const;
		int GetWindowsHeight() const;
		//�˳�ѭ��
		void Exit(int code);
		//�л�Scene��ע�⣬�ú���ֻ������Ϣ�������������Ϣ����һ֡�Ż��л���ʵ���л�������ISwitchScene��
		void SwitchScene(std::shared_ptr<Scene> scene);
		void AddScene(std::shared_ptr<Scene> scene);
		//��ȾScene
		void RenderScene();
		//Application��ܵ���
		LRESULT OnInputMessage(UINT msg, WPARAM wParam, LPARAM lParam) { return mInputManager.ProcessInputMessage(msg, wParam, lParam); }

		InputManager * GetInputManager() { return &mInputManager; }

		void SetOnCloseListener(std::function<bool(XGFramework &)> f) { mOnCloseListener = f; }
		void SetOnInputListener(std::function<bool(XGFramework &, const Event &ev)> f) { mOnInputListener = f; };
		UIBatches & GetUIBatches() { return mUiBatches; }
		EventDispatcher & GetEventDispatcher() { return mEventDispatcher; }
		struct AutoClose
		{
			bool operator()(XGFramework &)
			{
				return true;
			}
		};
	protected://�����ⲿ��ʼ���ı���
		GDI *mGDI;
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
		DISALLOW_COPY_AND_ASSIGN(XGFramework);
	};

};
