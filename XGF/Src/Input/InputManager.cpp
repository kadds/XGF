#include "..\..\Include\InputManager.hpp"
#include "../../Include/Log.hpp"


InputManager::InputManager():mForce(nullptr), mHasSetForce(false)
{
}


InputManager::~InputManager()
{
}

bool InputManager::Initialize(HINSTANCE hs, HWND hwnd, Asyn * a)
{
	if (!dinput.Initialize(hs, hwnd))
		return false;
	dinput.DoEvent(a);
	mHwnd = hwnd;
	CreateCaret(hwnd, NULL, 1, 20);
	ShowCaret(hwnd);
	SetCaretPos(0, 0);
	return true;
}

void InputManager::Shutdown()
{
	dinput.Shutdown();
	DestroyCaret();
}

LRESULT InputManager::ProcessInputMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (mForce == nullptr) break;
		if (wParam == VK_DELETE)
			mForce->Delete();
		else if (wParam == VK_LEFT)
			mForce->CursorToLeft();
		else  if (wParam == VK_RIGHT)
			mForce->CursorToRight();
		else if (wParam == VK_UP)
			mForce->CursorToUp();
		else  if (wParam == VK_DOWN)
			mForce->CursorToDowm();
		break;
	case WM_CHAR:
	{
		if (mForce == nullptr) break;
		if (wParam == VK_BACK)
		{
			mForce->BackUp();
		}
		else if (wParam == VK_ESCAPE || wParam == VK_LWIN || wParam == VK_RWIN || wParam == VK_TAB)
		{
		}
		else if(wParam == VK_RETURN)
			mForce->AppendInputStr(L'\n');
		else
			mForce->AppendInputStr(static_cast<wchar_t>(wParam));
		break;
	}
	default:
		break;
	}
	return 0;
}


void InputManager::OnActivate(bool isActivate)
{
	dinput.OnActivate(isActivate);
}

void InputManager::ClearForce(TextInputInterface * tei)
{
	auto ret = std::find(std::begin(mInputs), std::end(mInputs), tei);
	if (ret != std::end(mInputs))
		mInputs.erase(ret);
	if (mForce == tei)
		mForce = nullptr;
}
void InputManager::SetForce(TextInputInterface * tei) 
{
	if (tei != nullptr)
	{
		ShowCaret(mHwnd);
	}
	else
	{
		HideCaret(mHwnd);
		if (mForce != nullptr)
			mForce->OnForce(false);
		mForce = nullptr;
		return;
	}
	auto ret = std::find(std::begin(mInputs), std::end(mInputs), tei);
	if (ret == std::end(mInputs))
		mInputs.push_back(tei);
	for each (auto var in mInputs)
	{
		if (var != tei)
		{
			var->OnForce(false);
		}
		else
		{
			var->OnForce(true);
		}
	}
	mForce = tei;
	mHasSetForce = true;
}
void InputManager::StartForForce()
{
	mHasSetForce = false;
}
void InputManager::StopForForce()
{
	if (!mHasSetForce)
	{
		SetForce(nullptr);
	}
}
void InputManager::SetExclusiveMouseMode()
{
	dinput.SetExclusiveMode(true);
	dinput.SetRelativeMode(true);
}

void InputManager::SetNoExclusiveMouseMode()
{
	dinput.SetExclusiveMode(false);
	dinput.SetRelativeMode(false);
}

bool InputManager::IskeyDowm(Key k)
{
	return dinput.IsPress(k);
}
