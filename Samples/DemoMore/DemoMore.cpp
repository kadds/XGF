// TestMore.cpp : 定义应用程序的入口点。
//

#include "DemoMore.h"
#include "FirstScene.hpp"
#include "SecondScene.hpp"
#define MAX_LOADSTRING 100
#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib,"../Bin/x64/Debug/XGF.lib")
#else
#pragma comment(lib,"../Bin/Debug/XGF.lib")
#endif
#else
#ifdef _WIN64
#pragma comment(lib,"../Bin/x64/Release/XGF.lib")
#else
#pragma comment(lib,"../Bin/Release/XGF.lib")
#endif
#endif
// 全局变量: 
HINSTANCE hInst;             
// 此代码模块中包含的函数的前向声明: 

int RunGame(HINSTANCE hInstance);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	
    int p = 0;
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	int rt = RunGame(hInstance);
    return rt;
}
int RunGame(HINSTANCE hInstance)
{
	Application app;
	GDI gdi;
	FirstScene * gs = new FirstScene();
	XGFramework framework;
	framework.SetSceneDeleter([](Scene * sc) {delete sc;});
	framework.SetOnClose([]() {return true;});
	int rt = -1;
	rt = app.CreateWindowsAndRunApplication(framework, gdi, hInstance, L"Demo", L"DEC",
		0, 0, { 300,100 }, { 600,400 }, true, gs);
	return rt;
}
