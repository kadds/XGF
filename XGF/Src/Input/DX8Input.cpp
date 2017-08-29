#include "../../Include/DX8Input.hpp"
#include "../../Include/Log.hpp"
DX8Input::DX8Input()
{
}

DX8Input::~DX8Input()
{
}

bool DX8Input::Initialize(HINSTANCE hs, HWND hwnd)
{
	hInstance = hs;
	mHwnd = hwnd;
	Check(DirectInput8Create(hs, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&mDxInput, nullptr));
	//Check(dxInput->QueryInterface(IID_IDirectInputDevice2, (void **)&mDxInput));
	Check(mDxInput->CreateDevice(GUID_SysMouse, &mMouse, nullptr));
	Check(mDxInput->CreateDevice(GUID_SysKeyboard, &mKeyBoard, nullptr));
	// 设定为通过一个 256 字节的数组返回查询状态值
	Check(mKeyBoard->SetDataFormat(&c_dfDIKeyboard));
	Check(mKeyBoard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND));
	// 设定缓冲区大小
	// 如果不设定，缓冲区大小默认值为 0，程序就只能按立即模式工作
	// 如果要用缓冲模式工作，必须使缓冲区大小超过 0
	DIPROPDWORD     property;
	property.diph.dwSize = sizeof(DIPROPDWORD);
	property.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	property.diph.dwObj = 0;
	property.diph.dwHow = DIPH_DEVICE;
	property.dwData = 128;
	Check(mKeyBoard->SetProperty(DIPROP_BUFFERSIZE, &property.diph));

	Check(mMouse->SetDataFormat(&c_dfDIMouse));

	// 设定协作模式
	Check(mMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND));
	DIPROPDWORD     propertys;

	propertys.diph.dwSize = sizeof(DIPROPDWORD);
	propertys.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	propertys.diph.dwObj = 0;
	propertys.diph.dwHow = DIPH_DEVICE;
	propertys.dwData = 64;
	Check(mMouse->SetProperty(DIPROP_BUFFERSIZE, &propertys.diph));
	//propertys.dwData = DIPROPAXISMODE_ABS;
	//Check(mMouse->SetProperty(DIPROP_AXISMODE, &propertys.diph));

	mouseState.dowm = 0;
	memset(keys, 0, sizeof(keys));
	
	return true;
}

void DX8Input::Shutdown()
{
	PostThreadMessage(id, WM_QUIT, 0, 0);
	if (mDxInput)
	{
		if (mKeyBoard)
		{
			// Always unacquire the device before calling Release().
			mKeyBoard->Unacquire();
			mKeyBoard->Release();
			mKeyBoard = NULL;
		}
		if (mMouse)
		{
			// Always unacquire the device before calling Release().
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

void DX8Input::UpdatePos()
{
	//mKeyBoard->GetDeviceState(sizeof(keys),(LPVOID)keys);
	if (!mRelativeMode)
	{
		POINT p;
		p.x = mouseState.px;
		p.y = mouseState.py;
		GetCursorPos(&p);
		ScreenToClient(mHwnd, &p);
		mouseState.px = p.x;
		mouseState.py = p.y;
	}
	
}
void DX8Input::DoEvent(Asyn * asyn)
{
	mInputThread.DoAsyn([=](Asyn * as) {
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
		for(;;)
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
						DebugOut("DX8Input EXIT");
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
	if(Exclusive)
		mMouse->SetCooperativeLevel(mHwnd,
		DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	else
		mMouse->SetCooperativeLevel(mHwnd,
			DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
}
void DX8Input::SetRelativeMode(bool Relative)
{
	mRelativeMode = Relative;
	if (Relative) {
		if (!(mouseState.dowm & MOUSE_POSITION_RELATIVEMODE))
			mouseState.dowm |= MOUSE_POSITION_RELATIVEMODE;
	}
	else
	{
		if (mouseState.dowm & MOUSE_POSITION_RELATIVEMODE)
			mouseState.dowm |= MOUSE_POSITION_RELATIVEMODE;
	}

}

bool DX8Input::IsPress(Key key)
{
	return keys[key] & 0x80;
}

void DX8Input::HandleMouseEvent(DIDEVICEOBJECTDATA * didod, int len, Asyn * asyn)
{
	//memcpy(&lastmouseState, &mouseState, sizeof(MouseState));
	bool isDowm;
	for (int i = 0; i < len; i++)
	{
		switch (didod[i].dwOfs)
		{
		case DIMOFS_X:
			//hasMove = true;
			mouseState.px = didod[i].dwData;
			UpdatePos();
			asyn->PostEvent(EVENT_ONMOUSEMOVE, mouseState.px, mouseState.py, mouseState.dowm );
			break;
		case DIMOFS_Y:
			//hasMove = true;
			mouseState.py = didod[i].dwData;
			UpdatePos();
			asyn->PostEvent(EVENT_ONMOUSEMOVE, mouseState.px, mouseState.py, mouseState.dowm);
			break;
			case DIMOFS_Z:
				mouseState.pz = didod[i].dwData;
				asyn->PostEvent(EVENT_ONMOUSEMOVE, mouseState.px, mouseState.py, mouseState.dowm);
				break;
			case DIMOFS_BUTTON0:
				isDowm = static_cast<bool>(didod[i].dwData & 0x80);
				mouseState.dowms[0] = isDowm;
				if (isDowm)
				{
					mouseState.dowm |= MOUSE_BUTTON_LEFT;
					asyn->PostEvent(EVENT_ONMOUSEDOWM, mouseState.px, mouseState.py, MOUSE_BUTTON_LEFT);
				}
				else
				{
					mouseState.dowm ^= MOUSE_BUTTON_LEFT;
					asyn->PostEvent(EVENT_ONMOUSEUP, mouseState.px, mouseState.py, MOUSE_BUTTON_LEFT);
				}
				break;
			case DIMOFS_BUTTON1:
				isDowm = static_cast<bool>(didod[i].dwData & 0x80);
				mouseState.dowms[1] = isDowm;
				if (isDowm)
				{
					mouseState.dowm |= MOUSE_BUTTON_RIGHT;
					asyn->PostEvent(EVENT_ONMOUSEDOWM, mouseState.px, mouseState.py, MOUSE_BUTTON_RIGHT);
				}
				else
				{
					mouseState.dowm ^= MOUSE_BUTTON_RIGHT;
					asyn->PostEvent(EVENT_ONMOUSEUP, mouseState.px, mouseState.py, MOUSE_BUTTON_RIGHT);
				}
				break;
			case DIMOFS_BUTTON2:
				
				isDowm = static_cast<bool>(didod[i].dwData & 0x80);
				mouseState.dowms[2] = isDowm;
				if (isDowm)
				{
					mouseState.dowm |= MOUSE_BUTTON_MIDDLE;
					asyn->PostEvent(EVENT_ONMOUSEDOWM, mouseState.px, mouseState.py, MOUSE_BUTTON_MIDDLE);
				}
				else
				{
					mouseState.dowm ^= MOUSE_BUTTON_MIDDLE;
					asyn->PostEvent(EVENT_ONMOUSEUP, mouseState.px, mouseState.py, MOUSE_BUTTON_MIDDLE);
				}
				break;
			default:
				break;
			}
		}
}
void DX8Input::HandleKeyBoardEvent(DIDEVICEOBJECTDATA * didod, int len, Asyn * asyn)
{
	for (int i = 0; i< len; i++)
	{
		if (didod[i].dwData & 0x80)
		{
			keys[didod[i].dwOfs] |= 0x80;
			asyn->PostEvent(EVENT_ONKEYDOWM, didod[i].dwOfs, 0);
		}
		else
		{
			keys[didod[i].dwOfs] &= ~0x80;
			asyn->PostEvent(EVENT_ONKEYUP, didod[i].dwOfs, 0);
		}
 	}
}
void DX8Input::HandleInputMessage()
{
	mInputThread.HandleMessage();
}

