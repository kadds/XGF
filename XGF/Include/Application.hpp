#pragma once
#include <Windows.h>
#include "Defines.hpp"
#include <unordered_map>
#include "Asyn.hpp"
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
	//返回call framework Exit 的参数或call SetExitCode 的参数
	int CreateWindowsAndRunApplication(XGFramework &framework, GDI &gdi, HINSTANCE hInstance, const wchar_t * title, const wchar_t * className,
		int ICON, int sICON, POINT pos, SIZE size, bool CanChangeSize, Scene * firstScene);

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
	ATOM RegisterWindowsClass(HINSTANCE hInstance, const wchar_t * className, int ICON, int sICON);
	DISALLOW_COPY_AND_ASSIGN(Application);
};

#define WM_X_SHOWORHIDECURSOR WM_USER + 10