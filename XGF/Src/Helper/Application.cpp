#include "../../Include/Application.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/ShaderManager.hpp"

namespace XGF
{
	std::unordered_map<HWND, Application*> WndAppMap;
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	Application::Application()
	{
	}


	Application::~Application()
	{
	}

	int XGF::Application::CreateWindowsAndRunApplication(XGFramework & framework, GDI & gdi, HINSTANCE hInstance, WindowProperty windowProperty, std::shared_ptr<Scene> firstScene)
	{
		mInstance = hInstance;
		mFramework = &framework;
		SetProcessDPIAware();
		
		RegisterWindowsClass(hInstance, windowProperty.className, windowProperty.ICON, windowProperty.SICON);
		HWND hWnd = CreateWindowW(windowProperty.className, windowProperty.title, !windowProperty.canResize ? WS_OVERLAPPEDWINDOW &~WS_THICKFRAME & ~WS_MAXIMIZEBOX : WS_OVERLAPPEDWINDOW,
			windowProperty.point.x, windowProperty.point.y, windowProperty.size.cx, windowProperty.size.cy, nullptr, nullptr, hInstance, this);
		mHwnd = hWnd;
		if (!hWnd)
		{
			XGF_Error(Application, "An error occurred while creating a window");
			return false;
		}
		mSysCursor = LoadCursor(NULL, IDC_ARROW);
		//重设窗口客户区大小为传来的参数 大小
		RECT wndRc, clientRc;
		GetWindowRect(mHwnd, &wndRc);
		GetClientRect(mHwnd, &clientRc);
		int px, py;
		px = wndRc.right - wndRc.left - (clientRc.right - clientRc.left);

		py = wndRc.bottom - wndRc.top - (clientRc.bottom - clientRc.top);
		SetWindowPos(mHwnd, HWND_TOP, windowProperty.point.x, windowProperty.point.y, windowProperty.size.cx + px, windowProperty.size.cy + py, SWP_NOCOPYBITS | SWP_NOMOVE);
		XGF_Debug(Application, "ApplicationStart");
		gdi.Initialize(mInstance, mHwnd, mHwnd, clientRc.right - clientRc.left, clientRc.bottom - clientRc.top);

		mHideCursor = false;
		//开启渲染线程==============================================
		mRenderThread.DoAsyn(std::bind(&Application::RenderThreadStart, this, &gdi, std::move(firstScene)));
		//======================================================
		MSG msg;
		mRenderThread.Wait();
		ShowWindow(mHwnd, SW_SHOW);
		UpdateWindow(mHwnd);
		// 主消息循环: 
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, nullptr, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		XGF_Debug(Application, "ApplicationEnd");
		return exitCode;

	}

	Asyn& Application::GetRenderThread()
	{
		return mRenderThread;
	}

	XGFramework& Application::GetFramework()
	{
		return *mFramework;
	}

	bool Application::IsSetHideCursor()
	{
		return mHideCursor;
	}

	void Application::SetHideCursor(bool ishide)
	{
		mHideCursor = ishide;
	}

	HCURSOR Application::GetSysCursor()
	{
		return mSysCursor;
	}

	void Application::RenderThreadStart(GDI * gdi, std::shared_ptr<Scene> scene)
	{
		XGF_Debug(Framework, "Framework Start");
		ShaderManager shaderManager;
		Context::Initialize();
		Context & gameContext = Context::MakeContext(gdi, mFramework, &mRenderThread, &shaderManager);
		mFramework->_OnCreate();
		mFramework->AddScene(scene);
		mRenderThread.Notify();
		//消息循环
		XGF_Debug(Framework, "Framework Loop Start");
		mFramework->_Loop2();
		XGF_Debug(Framework, "Framework Destroy");
		shaderManager.ReleaseAll();
		mFramework->_OnDestroy();
		Context::ClearContext(gameContext);
		Context::Shutdown();
		//通知主线程退出
		mRenderThread.Notify();
		
	}

	ATOM Application::RegisterWindowsClass(HINSTANCE hInstance, const wchar_t * className, int ICON, int sICON)
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
	//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
	//
	//  目的:    处理主窗口的消息。
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
			//把消息交给InputManage处理
			app = WndAppMap.find(hWnd)->second;
			if (app != nullptr)
			{
				return app->GetFramework().OnInputMessage(message, wParam, lParam);
			}
			break;
		case WM_DESTROY:
			app = WndAppMap.find(hWnd)->second;
			if (app != nullptr)
			{
				XGF_Debug(Application, "WM_DESTROY IN");
				WndAppMap.erase(hWnd);
			}
			PostQuitMessage(0);
			break;
		case WM_ACTIVATE:
			app = WndAppMap.find(hWnd)->second;
			if (app != nullptr)
			{
				app->GetRenderThread().PostEvent(SystemEventId::Activate, { LOWORD(wParam) != WA_INACTIVE});
			}
			break;
		case WM_SIZE:
		{
			app = WndAppMap.find(hWnd)->second;
			RECT rc;
			GetClientRect(hWnd, &rc);
			if (app != nullptr && SIZE_MINIMIZED != wParam && SIZE_MAXHIDE != wParam)
			{
				XGF_Debug(Application, "WM_SIZE IN");
				app->GetRenderThread().PostWithoutRepeat(SystemEventId::Size, { static_cast<int>(rc.right - rc.left), static_cast<int>(rc.bottom - rc.top) });
			}
		}
		break;
		case WM_CREATE:
		{
			app = (Application *)((CREATESTRUCT*)lParam)->lpCreateParams;
			if (app != nullptr)
				WndAppMap.insert(std::make_pair(hWnd, app));
			break;
		}
		case WM_SETCURSOR:
		{
			app = WndAppMap.find(hWnd)->second;
			if (app != NULL)
			{
				if (app->IsSetHideCursor())
				{
					if (LOWORD(lParam) == HTCLIENT)
						SetCursor(NULL);
					else
						return DefWindowProc(hWnd, message, wParam, lParam);
				}
				else
				{
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			break;
		}
		case WM_CLOSE:
			app = WndAppMap.find(hWnd)->second;
			if (app != nullptr)
			{
				if (wParam == 1)
				{
					app->SetExitCode((int)lParam);
					app->GetRenderThread().Wait();
					XGF_Debug(Application, "Wait End Close Window");
					DestroyWindow(hWnd);
				}
				else
				{
					XGF_Debug(Application, "Send CloseMessage to System");
					app->GetRenderThread().PostEvent(SystemEventId::Close, { wParam != 0 });
				}
			}
			break;
		case WM_ENTERMENULOOP:
		case WM_ENTERSIZEMOVE:
		{
			app = WndAppMap.find(hWnd)->second;
			if (app != nullptr)
			{
				app->GetFramework().GetInputManager().OnActivate(!wParam);
			}
			break;
		}
		case WM_X_SHOWORHIDECURSOR:
		{
			app = WndAppMap.find(hWnd)->second;
			if (app != nullptr)
			{
				if (wParam == 0)//hide cursor
				{
					app->SetHideCursor(true);
					SetCursor(NULL);
				}
				else
				{
					app->SetHideCursor(false);
					SetCursor(app->GetSysCursor());
				}
			}

			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}
}
