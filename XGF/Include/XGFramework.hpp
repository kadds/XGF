#pragma once
#include "Timer.hpp"
#include "Defines.hpp"
#include "InputManager.hpp"
#include "InputListener.hpp"
#include "Asyn.hpp"
#include "SceneAnimation.hpp"
#include "Batch.hpp"
#include "Shader.hpp"
#include "OrthoCamera.hpp"
#include "Rectangle.hpp"
#include "RenderToTexture.hpp"
#include "ConstantData.hpp"
#include <list>
#include <memory>
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
		void _OnDestory();
		//Application��ܵ���
		void _OnActivate(bool isActivate);
		//Application��ܵ���
		void _OnSize(int ClientX, int ClientY);
		//Application��ܵ���
		void _OnMessage(const Event& ev);
		//��ܵ��ã��������Ȥ����Ϣ��ϵͳ��Ϣ��������
		virtual void OnMessage(const Event& ev) {};
		//Application��ܵ���
		bool _OnInput(const Event &ev);
		//Application��ܵ���
		void _OnClose();

		void Clear(float color[]);
		void Clear(Color &color);
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
		//�˳�ѭ��
		void Exit(int code);
		void AddInputListener(InputListener* il);
		void RemoveInputListener(InputListener* il);
		//�л�Scene��ע�⣬�ú���ֻ������Ϣ�������������Ϣ����һ֡�Ż��л���ʵ���л�������ISwithScene��
		//��ȷ����Click�¼��е��ò����ƻ�������
		void SwitchScene(Scene * scene);
		void AddScene(Scene * scene);
		//��ȾScene
		void RenderScene();
		//Application��ܵ���
		LRESULT OnInputMessage(UINT msg, WPARAM wParam, LPARAM lParam) { return mInputManager.ProcessInputMessage(msg, wParam, lParam); }

		InputManager * GetInputManager() { return &mInputManager; }

		void SetSceneDeleter(std::function<void(Scene *)> f) { mSceneDeleter = f; };
		void SetOnClose(std::function<bool()> f) { mOnClose = f; };
		void SetOnInput(std::function<bool(const Event &ev)> f) { mOnInput = f; };

	protected://�����ⲿ��ʼ���ı���
		GDI *mGDI;
	protected:
		Scene * mScene;
		Scene * mLastScene;
		void ISwitchScene(Scene * scene);
		Timer mainTimer;
		float mDeltaTime;
		InputManager mInputManager;
		std::list<InputListener* > mInputs;
		Asyn * mTheard;
		bool mIsVsync;
		std::function<void(Scene *)> mSceneDeleter;
		std::function<bool()> mOnClose;
		std::function<bool(const Event &ev)> mOnInput;
		RenderToTexture mRenderToTexture, mLastRenderToTexture;
		SceneAnimation * mSceneAnimation, *mLastSceneAnimation;
		Batch mSceneBatch;
		OrthoCamera mRenderCamera;
		Shape::Rectangle mRenderRectangle, mLastRenderRectangle;
	private:
		void DrawSceneAnimation();
		DISALLOW_COPY_AND_ASSIGN(XGFramework);
	};

};
