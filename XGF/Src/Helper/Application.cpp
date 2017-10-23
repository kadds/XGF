#include "../../Include/Application.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/Log.hpp"
#include "../../Include/GDI.hpp"
#include <crtdbg.h>
std::unordered_map<HWND,Application*> WndAppMap;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
Application::Application()
{
}


Application::~Application()
{
}

int Application::CreateWindowsAndRunApplication(XGFramework & framework, GDI & gdi, HINSTANCE hInstance, const wchar_t * title, const wchar_t * className, int ICON, int sICON, POINT pos, SIZE size)
{
	mInstance = hInstance;
	mFramework = &framework;
	EventPool::Initialize(100);
	RegisterWindowsClass(hInstance, className, ICON, sICON);
	HWND hWnd = CreateWindowW(className, title, WS_OVERLAPPEDWINDOW,
		pos.x, pos.y, size.cx, size.cy, nullptr, nullptr, hInstance, this);
	DebugOut("CreateWindowsEnd\n");
	mHwnd = hWnd;
	if (!hWnd)
	{
		return false;
	}
	//���贰�ڿͻ�����СΪ�����Ĳ��� ��С
	RECT wndRc, clientRc;
	GetWindowRect(mHwnd, &wndRc);
	GetClientRect(mHwnd, &clientRc);
	int px, py;
	px = wndRc.right - wndRc.left - (clientRc.right - clientRc.left);

	py = wndRc.bottom - wndRc.top - (clientRc.bottom - clientRc.top);
	SetWindowPos(mHwnd, HWND_TOP, pos.x, pos.y, size.cx + px, size.cy + py, SWP_NOCOPYBITS | SWP_NOMOVE);
	DebugOut("ApplicationStart\n");

	gdi.Initialize(mInstance, mHwnd, mHwnd, clientRc.right - clientRc.left, clientRc.bottom - clientRc.top);
	
	//������Ⱦ�߳�==============================================
	mRenderThread.DoAsyn(std::bind([this, &gdi](Asyn * RenderThread) {
		DebugOut("FrameworkStart\n");
		mFramework->_OnCreate(&gdi, RenderThread);
		RenderThread->Notify();
		//��Ϣѭ��
		DebugOut("FrameworkLoop\n");
		mFramework->_Loop2();
		DebugOut("FrameworkDestory\n");
		mFramework->_OnDestory();
		//֪ͨ���߳��˳�
		DebugOut("RenderThreadExit\n");
		RenderThread->Notify();
	}, std::placeholders::_1));
	//======================================================
	MSG msg;
	mRenderThread.Wait();
	ShowWindow(mHwnd, SW_SHOW);
	UpdateWindow(mHwnd);
	// ����Ϣѭ��: 
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, nullptr, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	DebugOut("ApplicationEnd\n");
	EventPool::Shutdown();
	return exitCode;

}

ATOM Application::RegisterWindowsClass(HINSTANCE hInstance,const wchar_t * className,int ICON,int sICON)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ICON));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(sICON));

	return RegisterClassExW(&wcex);
}
void Application::SetExitCode(int ec)
{
	exitCode = ec;
}
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Application * app;
	switch (message)
	{
	case WM_INPUT:
	case WM_IME_SETCONTEXT:
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_COMPOSITION:
	case WM_IME_NOTIFY:
	case WM_CHAR:
	case WM_KEYDOWN:
		//����Ϣ����InputManage����
		app = WndAppMap.find(hWnd)->second;
		if (app != nullptr && app->GetFramework() != nullptr)
		{
			return app->GetFramework()->OnInputMessage(message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		app = WndAppMap.find(hWnd)->second;
		if ( app != nullptr)
		{
            DebugOut("WM_DESTROY IN\n");
			WndAppMap.erase(hWnd);
		}
		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		app = WndAppMap.find(hWnd)->second;
		if ( app != nullptr)
		{
			app->GetRenderThread()->PostEvent(EVENT_ONACTIVATE, wParam == FALSE ? 0 : 1, 0);
		}
		break;
	case WM_SIZE:
	{
		app = WndAppMap.find(hWnd)->second;
		RECT rc;
		GetClientRect(hWnd, &rc);
		if ( app != nullptr)
		{
			DebugOut("WM_SIZE IN\n");
			app->GetRenderThread()->PostEvent(EVENT_ONSIZE, rc.right - rc.left, rc.bottom - rc.top);
		}
	}
	break;
	case WM_CREATE:
	{
		DebugOut("WM_CREATE\n");
		app = (Application *)((CREATESTRUCT*)lParam)->lpCreateParams;
		if (app != nullptr)
			WndAppMap.insert(std::make_pair(hWnd, app));
		break;
	}
	case WM_CLOSE:
		app = WndAppMap.find(hWnd)->second;
		if (app != nullptr)
		{
			if (wParam == 1)
			{
				app->SetExitCode((int)lParam);
				app->GetRenderThread()->Wait();
				DebugOut("Wait End\n")
				DestroyWindow(hWnd);
			}
			else
			{
				DebugOut("WM_CLOSE IN\n");
				app->GetRenderThread()->PostEvent(EVENT_ONCLOSE, wParam == FALSE ? 0 : 1, 0);
			}
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}