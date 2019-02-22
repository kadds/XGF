#pragma once
#include <Windows.h>
#include "Defines.hpp"
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
	class Context;
	class Application
	{
	public:
		Application();
		~Application();
		Application(const Application&) = delete;
		Application & operator = (const Application &) = delete;

		int CreateWindowsAndRunApplication(XGFramework &framework, GDI &gdi, HINSTANCE hInstance, WindowProperty windowProperty, std::shared_ptr<Scene> firstScene);

		Asyn & GetRenderThread();
		Asyn & GetGameThread();
		void SetExitCode(int ec);
		XGFramework& GetFramework();

		bool IsSetHideCursor();
		void SetHideCursor(bool ishide);;
		HCURSOR GetSysCursor();
		bool HasExitFlag() const;
	private:
		XGFramework *mFramework;
		HWND mHwnd;
		Asyn mRenderThread;
		Asyn mGameThread;
		HINSTANCE mInstance;
		int exitCode;
		bool mHideCursor;
		HCURSOR mSysCursor;
		int mExitFlag;
		ClassProperty(SizeEventFlag, int)
	private:
		void RenderThreadStart(Context * context);
		void GameThreadStart(Context * context, std::shared_ptr<Scene> scene);
		ATOM RegisterWindowsClass(HINSTANCE hInstance, const wchar_t * className, int ICON, int sICON);
	};

#define WM_X_SHOWORHIDECURSOR WM_USER + 10
}
