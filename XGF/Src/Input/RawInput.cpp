#include "../../Include/RawInput.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/Asyn.hpp"
#include "../../Include/Event.hpp"


namespace XGF::Input
{
	bool RawInput::Initialize()
	{
		auto hwnd = Context::Current().QueryGraphicsDeviceInterface().GetTopHwnd();

		RAWINPUTDEVICE rawinputDeice[2];
		rawinputDeice[0].dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY; //| RIDEV_NOLEGACY;
		rawinputDeice[0].usUsage = 0x6;
		rawinputDeice[0].usUsagePage = 0x1;
		rawinputDeice[0].hwndTarget = hwnd;

		
		rawinputDeice[1].dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY;
		rawinputDeice[1].usUsage = 0x2;
		rawinputDeice[1].usUsagePage = 0x1;
		rawinputDeice[1].hwndTarget = hwnd;

		if (RegisterRawInputDevices(rawinputDeice, 2, sizeof(RAWINPUTDEVICE)) == TRUE)
		{
			return true;
		}
		return false;
	}
	void RawInput::Shutdown()
	{
		auto hwnd = Context::Current().QueryGraphicsDeviceInterface().GetTopHwnd();

		RAWINPUTDEVICE rawinputDeice[2];
		rawinputDeice[0].dwFlags = RIDEV_REMOVE;
		rawinputDeice[0].usUsage = 0x6;
		rawinputDeice[0].usUsagePage = 0x1;
		rawinputDeice[0].hwndTarget = 0;


		rawinputDeice[1].dwFlags = RIDEV_REMOVE;
		rawinputDeice[1].usUsage = 0x2;
		rawinputDeice[1].usUsagePage = 0x1;
		rawinputDeice[1].hwndTarget = 0;

		RegisterRawInputDevices(rawinputDeice, 2, sizeof(RAWINPUTDEVICE));
	}
	void RawInput::SetExclusiveMode(bool isExclusiveMode)
	{

	}
	long RawInput::ProcessInputMessage(WPARAM wParam, LPARAM lParam)
	{
		std::unique_ptr<byte[]> iData;
		UINT dwSize;

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize,
			sizeof(RAWINPUTHEADER));
		iData = std::unique_ptr<byte[]>(new byte[dwSize]);

		auto& thread = Context::Current().QueryGameThread();
		unsigned int size = sizeof(RAWINPUTHEADER);
		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, iData.get(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
		{
			XGF_Warn(Input, "Error in GetRawInputData");
			return DefWindowProc(Context::Current().QueryGraphicsDeviceInterface().GetTopHwnd(), WM_INPUT, wParam, lParam);
		}
		auto * input = (RAWINPUT*)iData.get();
		if (input->header.dwType == RIM_TYPEMOUSE)
		{
			auto hwnd = Context::Current().QueryGraphicsDeviceInterface().GetTopHwnd();

			auto& mouse = input->data.mouse;
			
			if (mouse.lLastX != mMouseContent.GetPosX() || mouse.lLastY != mMouseContent.GetPosY())
			{
				mMouseContent.SetDeltaX(mouse.lLastX);
				mMouseContent.SetDeltaY(mouse.lLastY);
				POINT p;
				GetCursorPos(&p);
				ScreenToClient(hwnd, &p);

				mMouseContent.SetPosX(p.x);
				mMouseContent.SetPosY(p.y);
				thread.PostEvent(MouseEventId::MouseMove, { p.x, p.y, mMouseContent.GetDeltaX(), mMouseContent.GetDeltaY() });
			}
			
			if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
			{
				mMouseContent.Set(MouseKey::Left, true);
				thread.PostEvent(MouseEventId::MouseDown, { MouseKey::Left });
			}
			if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
			{
				mMouseContent.Set(MouseKey::Left, false);
				thread.PostEvent(MouseEventId::MouseUp, { MouseKey::Left });
			}
			if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
			{
				thread.PostEvent(MouseEventId::MouseDown, { MouseKey::Right });
				mMouseContent.Set(MouseKey::Right, true);
			}
			if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
			{
				mMouseContent.Set(MouseKey::Right, false);
				thread.PostEvent(MouseEventId::MouseUp, { MouseKey::Right });
			}
			if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
			{
				thread.PostEvent(MouseEventId::MouseDown, { MouseKey::Middle });
				mMouseContent.Set(MouseKey::Middle, true);
			}
			if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
			{
				mMouseContent.Set(MouseKey::Middle, false);
				thread.PostEvent(MouseEventId::MouseUp, { MouseKey::Middle });
			}
			if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN)
			{
				mMouseContent.Set(MouseKey::Button1, true);
				thread.PostEvent(MouseEventId::MouseDown, { MouseKey::Button1 });
			}
			if (mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP)
			{
				mMouseContent.Set(MouseKey::Button1, false);
				thread.PostEvent(MouseEventId::MouseUp, { MouseKey::Button1 });
			}
			if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN)
			{
				mMouseContent.Set(MouseKey::Button2, true);
				thread.PostEvent(MouseEventId::MouseDown, { MouseKey::Button2 });
			}
			if (mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP)
			{
				mMouseContent.Set(MouseKey::Button2, false);
				thread.PostEvent(MouseEventId::MouseUp, { MouseKey::Button2 });
			}
			if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN)
			{
				mMouseContent.Set(MouseKey::Button3, true);
				thread.PostEvent(MouseEventId::MouseDown, { MouseKey::Button3 });
			}
			if (mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP)
			{
				mMouseContent.Set(MouseKey::Button3, false);
				thread.PostEvent(MouseEventId::MouseUp, { MouseKey::Button3 });
			}
			if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
			{
				mMouseContent.Set(MouseKey::Button4, true);
				thread.PostEvent(MouseEventId::MouseDown, { MouseKey::Button4 });
			}
			if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
			{
				mMouseContent.Set(MouseKey::Button4, false);
				thread.PostEvent(MouseEventId::MouseUp, { MouseKey::Button4 });
			}
			if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
			{
				mMouseContent.Set(MouseKey::Button5, true);
				thread.PostEvent(MouseEventId::MouseDown, { MouseKey::Button5 });
			}
			if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
			{
				mMouseContent.Set(MouseKey::Button5, false);
				thread.PostEvent(MouseEventId::MouseUp, { MouseKey::Button5 });
			}
			if (mouse.usButtonFlags & RI_MOUSE_WHEEL)
			{
				mMouseContent.SetScroll((signed short)mouse.usButtonData);
				thread.PostEvent(MouseEventId::MouseWheel, { mMouseContent.GetScroll() });
			}
		}
		else if (input->header.dwType == RIM_TYPEKEYBOARD)
		{
			static int mPauseScancodeRead = 0;
			auto & keyboard = input->data.keyboard;
			bool e0 = keyboard.Flags & RI_KEY_E0;
			bool e1 = keyboard.Flags & RI_KEY_E1;

			unsigned int scanCode = GET_RAWINPUT_CODE_WPARAM(keyboard.MakeCode);
			if (keyboard.Flags & RI_KEY_E0)
				scanCode |= 0xE000;
			if (keyboard.Flags & RI_KEY_E1)
				scanCode |= 0xE100;
			if (mPauseScancodeRead) {
				if (scanCode == 0x45) {
					scanCode = 0xE11D45;
				}
				mPauseScancodeRead = 0;
			}
			else if (scanCode == 0xE11D) {
				mPauseScancodeRead = 1;
			}
			else if (scanCode == 0x54) {
				scanCode = 0xE037;
			}
			if (scanCode == 0xE11D || scanCode == 0xE02A || scanCode == 0xE0AA || scanCode == 0xE0B6 || scanCode == 0xE036) {
				// ignore it
				return DefWindowProc(Context::Current().QueryGraphicsDeviceInterface().GetTopHwnd(), WM_INPUT, wParam, lParam);
			}

			KeyBoardKey key = GetKeyFromScanCode(scanCode);
			
			if (keyboard.Flags & RI_KEY_BREAK)
			{
				mKeyboardContent.Set(key, false);
				thread.PostEvent(KeyBoardEventId::KeyUp, { key });
			}
			else
			{
				mKeyboardContent.Set(key, true);
				thread.PostEvent(KeyBoardEventId::KeyDown, { key });
			}
		}

		if (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT)
		{

		}
		else if (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUTSINK)
		{
			// background
		}

		return DefWindowProc(Context::Current().QueryGraphicsDeviceInterface().GetTopHwnd(), WM_INPUT, wParam, lParam);
	}
	long RawInput::ProcessInputDeviceChangedMessage(WPARAM wParam, LPARAM lParam)
	{
		if (wParam == GIDC_ARRIVAL)
		{
			// add
		}
		else if (wParam == GIDC_REMOVAL)
		{
			// remove
		}
		unsigned int size = 1024;
		wchar_t * name = new wchar_t[size];
		std::wstring devName;
		if (GetRawInputDeviceInfoW((HANDLE)lParam, RIDI_DEVICENAME, name, &size) < 0) 
		{
			XGF_Warn(Input, "buffer is too small than can't read input device name");
		}
		name[size - 1] = 0;
		devName = std::wstring(name);
		delete[] name;

		RID_DEVICE_INFO device_info = {};
		size = sizeof(RID_DEVICE_INFO);
		GetRawInputDeviceInfo((HANDLE)lParam, RIDI_DEVICEINFO, &device_info, &size);
		// TODO: send event to framework
		return 0;
	}
	const MouseContent& RawInput::GetMouseContent()
	{
		return mMouseContent;
	}
	const KeyboardContent& RawInput::GetKeyboardContent()
	{
		return mKeyboardContent;
	}
}