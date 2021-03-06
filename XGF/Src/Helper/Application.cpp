#include "../../Include/Application.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/ShaderManager.hpp"
#include "../../Include/Renderer.hpp"
#include "../../Include/Tools.hpp"
namespace XGF
{
	std::unordered_map<HWND, Application*> WndAppMap;
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	Application::Application(): mExitFlag(0), mSizeEventFlag(0)
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
		Tools::SetCurrentThreadName("Main Thread");
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
		//Reset window client area size to the passed parameter size
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
		Renderer renderer;
		ShaderManager shaderManager;
		Context::Initialize();
		Context & context = Context::MakeContext(&gdi, mFramework, &mGameThread, &mRenderThread, &renderer,&shaderManager);
		//Open render thread ==============================================
		mRenderThread.DoAsyn(std::bind(&Application::RenderThreadStart, this, &context));
		//======================================================
		mGameThread.DoAsyn(std::bind(&Application::GameThreadStart, this, &context, std::move(firstScene)));

		MSG msg;
		ShowWindow(mHwnd, SW_SHOW);
		UpdateWindow(mHwnd);
		// Main message loop 
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, nullptr, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		
		Context::ClearContext(context);
		Context::Shutdown();
		XGF_Debug(Application, "ApplicationEnd");
		UnregisterClass(windowProperty.className, hInstance);
		return exitCode;

	}

	Asyn& Application::GetRenderThread()
	{
		return mRenderThread;
	}

	Asyn& Application::GetGameThread()
	{
		return mGameThread;
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

	bool Application::HasExitFlag() const
	{
		return mExitFlag == 2;
	}

	void Application::RenderThreadStart(Context * context)
	{
		Tools::SetCurrentThreadName("Render Thread");
		SetThreadAffinityMask(GetCurrentThread(), 2);
		Context::JoinContext(*context);
		Context::Current().TagToRenderThread();
		ShaderManager & shaderManager = context->QueryShaderManager();

		context->QueryRenderer().Create();
		mGameThread.Notify();
		mRenderThread.Wait();
		context->QueryRenderer().Loop();
		mGameThread.Notify();
		mRenderThread.Wait();
		shaderManager.ReleaseAll();
		context->QueryRenderer().Destroy();
		Context::DetachContext();
		mExitFlag++;
	}

	void Application::GameThreadStart(Context* context, std::shared_ptr<Scene> scene)
	{
		Tools::SetCurrentThreadName("Game Thread");
		SetThreadAffinityMask(GetCurrentThread(), 1);
		Context::JoinContext(*context);
		XGF_Debug(Framework, "Framework Start");
		mGameThread.Wait();
		mFramework->_OnCreate();
		mFramework->AddScene(scene);
		XGF_Debug(Framework, "Framework Loop Start");

		mRenderThread.Notify();
		
		mFramework->_Loop2();
		mGameThread.Wait();
		XGF_Debug(Framework, "Framework Destroy");
		mFramework->_OnDestroy();
		mRenderThread.Notify();
		Context::DetachContext();
		mExitFlag++;
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

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Application * app;
		switch (message)
		{
		case WM_INPUT:
		case WM_INPUT_DEVICE_CHANGE:
		case WM_CHAR:
		case WM_MOUSEMOVE:
			app = WndAppMap.find(hWnd)->second;
			if (app != nullptr)
			{
				return app->GetFramework().OnInputMessage(hWnd, message, wParam, lParam);
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
			return 0;
		case WM_ACTIVATE:
			app = WndAppMap.find(hWnd)->second;
			if (app != nullptr)
			{
				app->GetRenderThread().PostEvent(SystemEventId::Activate, { LOWORD(wParam) != WA_INACTIVE});
				app->GetGameThread().PostEvent(SystemEventId::Activate, { LOWORD(wParam) != WA_INACTIVE });
				return 0;
			}
			break;
		case WM_SIZE:
		{
			app = WndAppMap.find(hWnd)->second;
			RECT rc;
			GetClientRect(hWnd, &rc);
			if (app != nullptr && SIZE_MINIMIZED != wParam && SIZE_MAXHIDE != wParam)
			{
				auto flag = app->GetSizeEventFlag();
				if(flag >= 1)
				{
					XGF_Debug(Application, "WM_SIZE IN");
					app->GetRenderThread().PostEvent(SystemEventId::Size, { static_cast<int>(rc.right - rc.left), static_cast<int>(rc.bottom - rc.top) });
					app->GetGameThread().PostEvent(SystemEventId::Size, { static_cast<int>(rc.right - rc.left), static_cast<int>(rc.bottom - rc.top) });
				}
				else
				{
					app->SetSizeEventFlag(flag + 1);
				}
			}
			return 0;
		}
		break;
		case WM_CREATE:
		{
			app = (Application *)((CREATESTRUCT*)lParam)->lpCreateParams;
			if (app != nullptr)
			{
				WndAppMap.insert(std::make_pair(hWnd, app));
				return 0;
			}
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
					while (!app->HasExitFlag())
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
					XGF_Debug(Application, "Wait End Close Window");
					DestroyWindow(hWnd);
				}
				else
				{
					XGF_Debug(Application, "Send CloseMessage to System");
					app->GetGameThread().PostEvent(SystemEventId::Close, { wParam != 0 });
				}
				return 0;
			}
			break;
		case WM_ENTERMENULOOP:
		case WM_ENTERSIZEMOVE:
		{
			app = WndAppMap.find(hWnd)->second;
			if (app != nullptr)
			{
				app->GetFramework().GetInputManager().OnActivate(!wParam);
				return 0;
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
			return 0;
		}
		default:
			break;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
