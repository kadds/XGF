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
	此Helper类用于建立一个D3D窗口
	同时与渲染线程通信
	*/
	class Application
	{
	public:
		Application();
		~Application();
		Application(const Application&) = delete;
		Application & operator = (const Application &) = delete;
		//返回call framework Exit 的参数或call SetExitCode 的参数
		int CreateWindowsAndRunApplication(XGFramework &framework, GDI &gdi, HINSTANCE hInstance, WindowProperty windowProperty, std::shared_ptr<Scene> firstScene);

		Asyn& GetRenderThread();
		void SetExitCode(int ec);
		XGFramework& GetFramework();

		bool IsSetHideCursor();
		void SetHideCursor(bool ishide);;
		HCURSOR GetSysCursor();
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
	};

#define WM_X_SHOWORHIDECURSOR WM_USER + 10
}
