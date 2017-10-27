#pragma once
#include <Windows.h>
#include "Defines.hpp"
#include <unordered_map>
#include "Asyn.hpp"
class XGFramework;
class GDI;
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
