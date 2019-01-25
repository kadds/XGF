#include "../../Include/DX8Input.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/Batch.hpp"
#include "../../Include/Context.hpp"

namespace XGF
{
	DX8Input::DX8Input()
	{
	}

	DX8Input::~DX8Input()
	{
	}

	bool DX8Input::Initialize()
	{
		auto & context = Context::Current();
		HWND hwnd = context.QueryGraphicsDeviceInterface().GetTopHwnd();
		auto hs = context.QueryGraphicsDeviceInterface().GetInstance();
		XGF_ASSERT(hwnd != NULL);
		XGF_ASSERT(hs != NULL);
		hInstance = hs;
		mHwnd = hwnd;
		XGF_Error_Check(Input, DirectInput8Create(hs, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&mDxInput, nullptr),"DxInputCreate Failed");
		//²éÑ¯
		//Check(dxInput->QueryInterface(IID_IDirectInputDevice2, (void **)&mDxInput));
		XGF_Error_Check(Input, mDxInput->CreateDevice(GUID_SysMouse, &mMouse, nullptr),"DxInputCreateDevice Failed");
		XGF_Error_Check(Input, mDxInput->CreateDevice(GUID_SysKeyboard, &mKeyBoard, nullptr), "DxInputCreateDevice Failed");

		XGF_Error_Check(Input, mKeyBoard->SetDataFormat(&c_dfDIKeyboard), "DxInput SetDataFormat Failed");
		XGF_Error_Check(Input, mKeyBoard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND), "SetCooperativeLevel Failed");

		DIPROPDWORD     property;
		property.diph.dwSize = sizeof(DIPROPDWORD);
		property.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		property.diph.dwObj = 0;
		property.diph.dwHow = DIPH_DEVICE;
		property.dwData = 128;
		XGF_Error_Check(Input, mKeyBoard->SetProperty(DIPROP_BUFFERSIZE, &property.diph), "SetKeyBoardProperty Failed");

		XGF_Error_Check(Input, mMouse->SetDataFormat(&c_dfDIMouse2), "SetDataFormat Failed");

		XGF_Error_Check(Input, mMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND), "SetMouseCooperativeLevel Failed");
		DIPROPDWORD     propertys;

		propertys.diph.dwSize = sizeof(DIPROPDWORD);
		propertys.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		propertys.diph.dwObj = 0;
		propertys.diph.dwHow = DIPH_DEVICE;
		propertys.dwData = 64;
		XGF_Error_Check(Input, mMouse->SetProperty(DIPROP_BUFFERSIZE, &propertys.diph), "SetMouseProperty Failed");
		//propertys.dwData = DIPROPAXISMODE_ABS;
		//Check(mMouse->SetProperty(DIPROP_AXISMODE, &propertys.diph));
		POINT p;
		p.x = mouseState.px;
		p.y = mouseState.py;
		GetCursorPos(&p);
		ScreenToClient(mHwnd, &p);
		mouseState.px = p.x;
		mouseState.py = p.y;
		mouseState.pz = 0;
		mouseState.dowm = 0;
		RECT rc;
		GetClientRect(hwnd, &rc);
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;
		memset(keys, 0, sizeof(keys));
		mMoveable = true;
		mInvalid = false;
		XGF_Debug(Input, "d8input subsystem initialized");
		return true;
	}

	void DX8Input::Shutdown()
	{
		XGF_Debug(Input, "d8input subsystem shutdown");
		PostThreadMessage(id, WM_QUIT, 0, 0);
		if (mDxInput)
		{
			if (mKeyBoard)
			{
				mKeyBoard->Unacquire();
				mKeyBoard->Release();
				mKeyBoard = NULL;
			}
			if (mMouse)
			{
				mMouse->Unacquire();
				mMouse->Release();
				mMouse = NULL;
			}
			mDxInput->Release();
			mDxInput = NULL;
		}
	}

	void DX8Input::OnActivate(bool isActivate)
	{
		if (isActivate)
		{
			mKeyBoard->Acquire();
			mMouse->Acquire();
		}
		else
		{
			mKeyBoard->Unacquire();
			mMouse->Unacquire();
		}
	}

	void DX8Input::DoEvent(Asyn * asyn)
	{
		mInputThread.DoAsyn([=](Asyn * as) {
			Tools::SetCurrentThreadName("DX8Input Thread");
			MSG msg{};
			HANDLE had[2];

			id = GetCurrentThreadId();
			had[0] = CreateEvent(nullptr, FALSE, FALSE, FALSE);
			had[1] = CreateEvent(nullptr, FALSE, FALSE, FALSE);
			mMouse->SetEventNotification(had[0]);
			mKeyBoard->SetEventNotification(had[1]);
			DIDEVICEOBJECTDATA  didod[128];  // Receives buffered data
			DWORD    dwElements = 128;
			HRESULT ret;
			for (;;)
			{
				auto result = MsgWaitForMultipleObjects(2, had, FALSE, INFINITE, QS_ALLINPUT);
				switch (result)
				{
				case WAIT_OBJECT_0:
					mMouse->Acquire();
					ZeroMemory(didod, sizeof(didod));
					dwElements = 64;
					ret = mMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0);
					if (ret == DIERR_INPUTLOST)
					{
						mMouse->Acquire();
						dwElements = 64;
						mMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0);
					}
					HandleMouseEvent(didod, dwElements, asyn);
					break;
				case WAIT_OBJECT_0 + 1:
					mKeyBoard->Acquire();
					ZeroMemory(didod, sizeof(didod));
					dwElements = 128;
					ret = mKeyBoard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0);
					if (ret == DIERR_INPUTLOST)
					{
						mKeyBoard->Acquire();
						dwElements = 128;
						mKeyBoard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &dwElements, 0);
					}
					HandleKeyBoardEvent(didod, dwElements, asyn);
					break;
				case WAIT_OBJECT_0 + 2:
					while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
						if (msg.message == WM_QUIT) {
							//DebugOut("DX8Input EXIT");
							return;
						}
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					break;
				default:
					break;
				}
			}
		});

	}
	void DX8Input::SetExclusiveMode(bool Exclusive)
	{
		HRESULT hr;
		if (Exclusive)
			hr = mMouse->SetCooperativeLevel(mHwnd,
				DISCL_EXCLUSIVE);
		else
			hr = mMouse->SetCooperativeLevel(mHwnd,
				DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		if (hr != S_OK)
			XGF_Warn(Input, "DXinput SetExclusiveMode Failed");
	}
	void DX8Input::SetRelativeMode(bool Relative)
	{
		mRelativeMode = Relative;
		if (Relative) {
			if (!(mouseState.dowm & MOUSE_POSITION_RELATIVEMODE))
				mouseState.dowm |= MOUSE_POSITION_RELATIVEMODE;
			mouseState.px = mouseState.py = mouseState.pz = 0;
		}
		else
		{
			if (mouseState.dowm & MOUSE_POSITION_RELATIVEMODE)
				mouseState.dowm |= MOUSE_POSITION_RELATIVEMODE;
		}

	}

	bool DX8Input::IsPress(Key key)
	{
		return keys[key];
	}
	bool DX8Input::GetAbsolutePos()
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(mHwnd, &p);
		mouseState.px = p.x;
		mouseState.py = p.y;
		return true;
	}

#pragma warning(push)
#pragma warning(disable:4644)
	void DX8Input::HandleMouseEvent(DIDEVICEOBJECTDATA * didod, int len, Asyn * asyn)
	{
		bool isDown;
		if (!mMoveable || mInvalid) {
			return;
		}
		if (mRelativeMode)
		{
			auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
			POINT p;
			p.x = static_cast<LONG>(gdi.GetWidth() / 2);
			p.y = static_cast<LONG>(gdi.GetHeight() / 2);
			ClientToScreen(mHwnd, &p);
			SetCursorPos(p.x, p.y);
		}
		for (int i = 0; i < len; i++)
		{
			switch (didod[i].dwOfs)
			{
			case DIMOFS_X:
				if (mRelativeMode)
					mouseState.px = didod[i].dwData;
				else
				{
					if (!GetAbsolutePos())
						break;
				}
				asyn->PostEvent(MouseEventId::MouseMove, { mouseState.px, mouseState.py, mouseState.dowm });
				break;
			case DIMOFS_Y:
				if (mRelativeMode)
					mouseState.py = didod[i].dwData;
				else
				{
					if(!GetAbsolutePos())
						break;
				}
				asyn->PostEvent(MouseEventId::MouseMove, { mouseState.px, mouseState.py, mouseState.dowm });
				break;
			case DIMOFS_Z:
				mouseState.pz = didod[i].dwData;
				asyn->PostEvent(MouseEventId::MouseWheel, { mouseState.pz});
				break;
			case DIMOFS_BUTTON0:
				isDown = static_cast<bool>(didod[i].dwData & 0x80);
				mouseState.dowms[0] = isDown;
				if (isDown)
				{
					mouseState.dowm |= MOUSE_BUTTON_LEFT;
					asyn->PostEvent(MouseEventId::MouseDown, { mouseState.px, mouseState.py, MOUSE_BUTTON_LEFT });
				}
				else
				{
					mouseState.dowm ^= MOUSE_BUTTON_LEFT;
					asyn->PostEvent(MouseEventId::MouseUp, {mouseState.px, mouseState.py, MOUSE_BUTTON_LEFT});
				}
				break;
			case DIMOFS_BUTTON1:
				isDown = static_cast<bool>(didod[i].dwData & 0x80);
				mouseState.dowms[1] = isDown;
				if (isDown)
				{
					mouseState.dowm |= MOUSE_BUTTON_RIGHT;
					asyn->PostEvent(MouseEventId::MouseDown, { mouseState.px, mouseState.py, MOUSE_BUTTON_RIGHT });
				}
				else
				{
					mouseState.dowm ^= MOUSE_BUTTON_RIGHT;
					asyn->PostEvent(MouseEventId::MouseUp, { mouseState.px, mouseState.py, MOUSE_BUTTON_RIGHT });
				}
				break;
			case DIMOFS_BUTTON2:

				isDown = static_cast<bool>(didod[i].dwData & 0x80);
				mouseState.dowms[2] = isDown;
				if (isDown)
				{
					mouseState.dowm |= MOUSE_BUTTON_MIDDLE;
					asyn->PostEvent(MouseEventId::MouseDown, { mouseState.px, mouseState.py, MOUSE_BUTTON_MIDDLE });
				}
				else
				{
					mouseState.dowm ^= MOUSE_BUTTON_MIDDLE;
					asyn->PostEvent(MouseEventId::MouseUp, { mouseState.px, mouseState.py, MOUSE_BUTTON_MIDDLE });
				}
				break;
			default:
				break;
			}
		}
	}
#pragma warning(pop)
	void DX8Input::HandleKeyBoardEvent(DIDEVICEOBJECTDATA * didod, int len, Asyn * asyn)
	{
		for (int i = 0; i< len; i++)
		{
			if (didod[i].dwData & 0x80)
			{
				keys[didod[i].dwOfs] = true;
				asyn->PostEvent(KeyBoardEventId::KeyDown, { (int)didod[i].dwOfs });
			}
			else
			{
				keys[didod[i].dwOfs] = false;
				asyn->PostEvent(KeyBoardEventId::KeyUp, { (int)(didod[i].dwOfs) });
			}
		}
	}
	void DX8Input::HandleInputMessage()
	{
		mInputThread.HandleMessage();
	}


}
