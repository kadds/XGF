#include "..\..\Include\InputManager.hpp"
#include "../../Include/Application.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/GDI.hpp"

namespace XGF::Input
{
	InputManager::InputManager()
	{
	}


	InputManager::~InputManager()
	{
	}

	void InputManager::Initialize()
	{

		SetRelativeMouse(false);
		if (!SwitchToRawInput() && !IsDInput() && !IsRawInput())
		{
			XGF_Error(Input, "Can't Initialize input device!");
		}
		SetMouseMode(MouseMode::Default);
	}

	void InputManager::Shutdown()
	{
		if (dinput)
		{
			dinput->Shutdown();
			dinput.reset();
		}
		if (rawinput)
		{
			rawinput->Shutdown();
			rawinput.reset();
		}
	}
	void InputManager::Tick(float delta)
	{
		if (rawinput)
		{
			mCurrentMouseContent = rawinput->GetMouseContent();
			mCurrentKeyBoardContent = rawinput->GetKeyboardContent();
		}
		else
		{
			mCurrentMouseContent = dinput->GetMouseContent();
			mCurrentKeyBoardContent = dinput->GetKeyboardContent();
		}
	}
	// main thread
	LRESULT InputManager::ProcessInputMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CHAR:
			Context::Current().QueryGameThread().PostEvent(KeyBoardEventId::Char, { (unsigned int)wParam });
			break;
		case WM_INPUT:
			if (rawinput)
			{
				return rawinput->ProcessInputMessage(wParam, lParam);
			}
		case WM_INPUT_DEVICE_CHANGE:
			if (rawinput)
			{
				return rawinput->ProcessInputDeviceChangedMessage(wParam, lParam);
			}
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		return 0;
	}


	void InputManager::OnActivate(bool isActivate)
	{
		if (dinput)
			dinput->OnActivate(isActivate);
	}

	void InputManager::UpdateCameraMatrix(int x, int y)
	{
		if (dinput)
			dinput->UpdateSize(x, y);
	}


	void InputManager::SetMouseMode(MouseMode mm)
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		auto w = gdi.GetWidth() / 2, h = gdi.GetHeight() / 2;
		auto hwnd = gdi.GetTopHwnd();
		switch (mm)
		{
		case MouseMode::Default:
			PostMessage(hwnd, WM_X_SHOWORHIDECURSOR, TRUE, 0);
			if (dinput)
				dinput->SetExclusiveMode(false);
			break;
		case MouseMode::Exclusive:
			PostMessage(hwnd, WM_X_SHOWORHIDECURSOR, FALSE, 0);
			if (dinput)
				dinput->SetExclusiveMode(true);
			break;
		default:
			break;
		}
		mMouseMode = mm;
	}
	void InputManager::EnableXInput()
	{
		if (!xinput) 
		{
			xinput = std::make_unique<XInput>();
			xinput->Initialize();
		}
	}
	void InputManager::DisableXInput()
	{
		if (xinput)
		{
			xinput->Shutdown();
			xinput.reset();
		}
	}
	bool InputManager::SwitchToRawInput()
	{
		if (!rawinput) 
		{
			rawinput = std::make_unique<RawInput>();
			if (!rawinput->Initialize())
			{
				rawinput.reset();
				return false;
			}
		}
		if (dinput)
		{
			dinput->Shutdown();
			dinput.reset();
		}
		return true;
	}
	bool InputManager::SwitchToDInput()
	{
		if (!dinput)
		{
			dinput = std::make_unique<DX8Input>();
			if (!dinput->Initialize())
			{
				dinput.reset();
				return false;
			}
		}
		if (rawinput)
		{
			rawinput->Shutdown();
			rawinput.reset();
		}
		return true;
	}
	bool InputManager::IsRawInput()
	{
		return rawinput ? true : false;
	}
	bool InputManager::IsDInput()
	{
		return dinput ? true : false;
	}
	bool InputManager::IsXInputEnable()
	{
		return xinput ? true : false;
	}

	const MouseContent& InputManager::GetMouseContent()
	{
		return mCurrentMouseContent;
	}

	const KeyboardContent& InputManager::GetKeyboardContent()
	{
		return mCurrentKeyBoardContent;
	}

	bool InputManager::IskeyDown(MouseKey k)
	{
		return GetMouseContent().Get(k);
	}

	bool InputManager::IskeyDown(KeyBoardKey k)
	{
		return GetKeyboardContent().Get(k);
	}

	bool InputManager::IsRelativeMouse() const
	{
		return mIsRelativeMouse;
	}

	void InputManager::SetRelativeMouse(bool isRelativeMouse)
	{
		mIsRelativeMouse = isRelativeMouse;
	}

}
