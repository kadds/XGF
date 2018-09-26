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
		void Present(bool isVsync) const;

		void OpenVsync() { mIsVsync = true; }
		void CloseVsync() { mIsVsync = false; }
		HWND GetTopHwnd() const;
		HINSTANCE GetInstance() const;
		Asyn * GetTheard() const { return mTheard; }
		GDI * GetGDI() const;

		int GetWindowsWidth() const;
		int GetWindowsHeight() const;
		//�˳�ѭ��
		void Exit(int code);
		//�л�Scene��ע�⣬�ú���ֻ������Ϣ�������������Ϣ����һ֡�Ż��л���ʵ���л�������ISwithScene��
		//��ȷ����Click�¼��е��ò����ƻ�������
		void SwitchScene(std::shared_ptr<Scene> scene);
		void AddScene(std::shared_ptr<Scene> scene);
		//��ȾScene
		void RenderScene();
		//Application��ܵ���
		LRESULT OnInputMessage(UINT msg, WPARAM wParam, LPARAM lParam) { return mInputManager.ProcessInputMessage(msg, wParam, lParam); }

		InputManager * GetInputManager() { return &mInputManager; }

		void SetOnCloseListener(std::function<bool(XGFramework &)> f) { mOnCloseListener = f; }
		void SetOnInputListener(std::function<bool(XGFramework &, const Event &ev)> f) { mOnInputListener = f; };

		EventDispatcher & GetEventDispatcher() { return mEventDispatcher; }

		UIBatches & GetUIBatches() { return mUIBatches; };
	protected://�����ⲿ��ʼ���ı���
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

		std::function<bool(XGFramework &)> mOnCloseListener;
		std::function<bool(XGFramework &, const Event &ev)> mOnInputListener;

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
