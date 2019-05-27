#include "../../Include/DX8Input.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/GDI.hpp"

namespace XGF::Input
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

		XGF_Error_Check(Input, DirectInput8Create(hs, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&mDxInput, nullptr),"DxInputCreate Failed");
		
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
		GetCursorPos(&p);
		ScreenToClient(hwnd, &p);
		mMouseContent.SetPosX(p.x);
		mMouseContent.SetPosY(p.y);
		mMouseContent.SetDeltaX(0);
		mMouseContent.SetDeltaY(0);

		RECT rc;
		GetClientRect(hwnd, &rc);
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;


		XGF_Debug(Input, "d8input subsystem initialized");
		DoEvent();
		return true;
	}

	void DX8Input::Shutdown()
	{
		XGF_Debug(Input, "d8input subsystem shutdown");
		auto id = mInputThread.GetThreadId();
		DWORD ids = *((DWORD*)(&id));
		PostThreadMessage(ids, WM_QUIT, 0, 0);
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

	void DX8Input::DoEvent()
	{
		Context& context = Context::Current();
		mInputThread.DoAsyn([&context, this](Asyn * asyn) {
			Context::JoinContext(context);
			Tools::SetCurrentThreadName("DX8Input Thread");
			MSG msg{};
			HANDLE had[2];

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
					HandleMouseEvent(didod, dwElements);
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
					HandleKeyBoardEvent(didod, dwElements);
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
		auto hwnd = Context::Current().QueryGraphicsDeviceInterface().GetTopHwnd();

		HRESULT hr;
		if (Exclusive)
			hr = mMouse->SetCooperativeLevel(hwnd,
				DISCL_EXCLUSIVE);
		else
			hr = mMouse->SetCooperativeLevel(hwnd,
				DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		if (hr != S_OK)
			XGF_Warn(Input, "DXinput SetExclusiveMode Failed");
	}


	const MouseContent& DX8Input::GetMouseContent()
	{
		return mMouseContent;
	}
	const KeyboardContent& DX8Input::GetKeyboardContent()
	{
		return mKeyboardContent;
	}
#pragma warning(push)
#pragma warning(disable:4644)
	void DX8Input::HandleMouseEvent(DIDEVICEOBJECTDATA * didod, int len)
	{
		auto& gameThread = Context::Current().QueryGameThread();

		if (mInvalid) {
			return;
		}
		auto& gdi = Context::Current().QueryGraphicsDeviceInterface();
		MouseContent content;
		std::bitset<10> hasset;
		for (int i = 0; i < len; i++)
		{
			switch (didod[i].dwOfs)
			{
			case DIMOFS_X:
				content.SetDeltaX(didod[i].dwData);
				hasset.set(0);
				break;
			case DIMOFS_Y:
				hasset.set(0);
				content.SetDeltaY(didod[i].dwData);
				break;
			case DIMOFS_Z:
				hasset.set(1);
				content.SetScroll(didod[i].dwData);
				break;
			case DIMOFS_BUTTON0:
			case DIMOFS_BUTTON1:
			case DIMOFS_BUTTON2:
			case DIMOFS_BUTTON3:
			case DIMOFS_BUTTON4:
			case DIMOFS_BUTTON5:
			case DIMOFS_BUTTON6:
			case DIMOFS_BUTTON7:
				hasset.set((int)(didod[i].dwOfs - DIMOFS_BUTTON0 + 2));
				content.Set(MouseKey(didod[i].dwOfs - DIMOFS_BUTTON0), (bool)(didod[i].dwData & 0x80));
				break;
			default:
				break;
			}
		}
		if (hasset.test(0)) // move
		{
			auto hwnd = Context::Current().QueryGraphicsDeviceInterface().GetTopHwnd();
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(hwnd, &p);
			mMouseContent.SetPosX(p.x);
			mMouseContent.SetPosY(p.y);
			mMouseContent.SetDeltaX(content.GetDeltaX());
			mMouseContent.SetDeltaY(content.GetDeltaY());
			gameThread.PostEvent(MouseEventId::MouseMove, { mMouseContent.GetPosX(), mMouseContent.GetPosY(), mMouseContent.GetDeltaX(), mMouseContent.GetDeltaY() });
		}
		if (hasset.test(1)) //scroll
		{
			mMouseContent.SetScroll(content.GetScroll());
			gameThread.PostEvent(MouseEventId::MouseWheel, { mMouseContent.GetScroll() });
		}
		for (int i = 2; i < 10; i++)
		{
			if (hasset.test(i))
			{
				bool down = content.Get((MouseKey)(i - 2));
				gameThread.PostEvent(down ? MouseEventId::MouseDown : MouseEventId::MouseUp
					, { (MouseKey)(i - 2) });
			}
		}
	}
#pragma warning(pop)
	void DX8Input::HandleKeyBoardEvent(DIDEVICEOBJECTDATA * didod, int len)
	{
		auto& gameThread = Context::Current().QueryGameThread();

		for (int i = 0; i< len; i++)
		{
			KeyBoardKey keyCode = GetKeyFromDIK((unsigned char)didod[i].dwOfs);
			if (didod[i].dwData & 0x80)
			{
				mKeyboardContent.Set(keyCode, true);
				gameThread.PostEvent(KeyBoardEventId::KeyDown, { keyCode });
			}
			else
			{
				mKeyboardContent.Set(keyCode, false);
				gameThread.PostEvent(KeyBoardEventId::KeyUp, { keyCode });
			}
		}
	}
	void DX8Input::HandleInputMessage()
	{
		mInputThread.HandleMessage();
	}
}
