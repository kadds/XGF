#pragma once
#include <Windows.h>
#include "Defines.hpp"
#include <unordered_map>
#include "Asyn.hpp"
namespace XGF
{
	struct WindowProperty
	{
		const wchar_t * title;
		const wchar_t * className;
		POINT point;
		SIZE size;
		int ICON;
		int SICON;
		bool canResize;
	};
	class XGFramework;
	class GDI;
	class Scene;
	/*
	��Helper�����ڽ���һ��D3D����
	ͬʱ����Ⱦ�߳�ͨ��
	*/
	class Application
	{
	public:
		Application();
		~Application();
		//����call framework Exit �Ĳ�����call SetExitCode �Ĳ���
		int CreateWindowsAndRunApplication(XGFramework &framework, GDI &gdi, HINSTANCE hInstance, WindowProperty windowProperty, std::shared_ptr<Scene> firstScene);

		Asyn * GetRenderThread() { return &mRenderThread; }
		void SetExitCode(int ec);
		XGFramework * GetFramework() { return mFramework; }

		bool IsSetHideCursor() { return mHideCursor; }
		void SetHideCursor(bool ishide) { mHideCursor = ishide; };
		HCURSOR GetSysCursor() { return mSysCursor; }
	private:
		XGFramework *mFramework;
		HWND mHwnd;
		Asyn mRenderThread;
		HINSTANCE mInstance;
		int exitCode;
		bool mHideCursor;
		HCURSOR mSysCursor;
	private:
		void RenderThreadStart(GDI * gdi, std::shared_ptr<Scene> scene);
		ATOM RegisterWindowsClass(HINSTANCE hInstance, const wchar_t * className, int ICON, int sICON);
		DISALLOW_COPY_AND_ASSIGN(Application);
	};

#define WM_X_SHOWORHIDECURSOR WM_USER + 10
}
