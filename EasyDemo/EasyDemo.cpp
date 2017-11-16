#include "EasyDemo.h"
#include "GameScene.h"

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#define new  new(_CLIENT_BLOCK, __FILE__, __LINE__)  
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
int RunGame(HINSTANCE hInstance);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
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
	GameScene * gs = new GameScene();
	XGFramework framework;
	framework.SetSceneDeleter([](Scene * sc) {delete sc;});
	framework.SetOnClose([]() {return true;});
	int rt = -1;
	rt = app.CreateWindowsAndRunApplication(framework, gdi, hInstance, L"tet", L"dsgdfhv,",
		0, 0, { 300, 100 }, { 600, 400 }, false, gs);
	return rt;
}