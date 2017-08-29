#pragma once
#include <Windows.h>
#include "Defines.hpp"
#include <unordered_map>
#include "Asyn.hpp"
class XGFramework;
class GDI;
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
	int CreateWindowsAndRunApplication(XGFramework &framework, GDI &gdi, HINSTANCE hInstance, const wchar_t * title, const wchar_t * className, int ICON, int sICON, POINT pos, SIZE size);

	Asyn * GetRenderThread() { return &mRenderThread; }
	void SetExitCode(int ec);
	XGFramework * GetFramework() { return mFramework; }
private:
	XGFramework *mFramework;
	HWND mHwnd;
	Asyn mRenderThread;
	HINSTANCE mInstance;
	int exitCode;
private:
	ATOM RegisterWindowsClass(HINSTANCE hInstance, const wchar_t * className, int ICON, int sICON);
	DISALLOW_COPY_AND_ASSIGN(Application);
};

