#include "..\..\Include\InputManager.hpp"
#include "../../Include/Log.hpp"


InputManager::InputManager():mForce(nullptr), mHasSetForce(false)
{
}


InputManager::~InputManager()
{
}

bool InputManager::Initialize(GDI * gdi, HINSTANCE hs, HWND hwnd, Asyn * a)
{
	if (!dinput.Initialize(hs, hwnd))
		return false;
	dinput.DoEvent(a);
	mHwnd = hwnd;
	mCaret.Initialize(gdi);
	mCursor.Initialize(gdi);
	mCaretPosInText = 0;
	return true;
}

void InputManager::Shutdown()
{
	mCaret.Shutdown();
	mCursor.Shutdown();
	dinput.Shutdown();
}

LRESULT InputManager::ProcessInputMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (mForce == nullptr) break;
		if (wParam == VK_DELETE)
			mForce->Delete(), mCaret.ResetTime();
		else if (wParam == VK_LEFT)
			mForce->CaretToLeft(), mCaret.ResetTime();
		else  if (wParam == VK_RIGHT)
			mForce->CaretToRight(), mCaret.ResetTime();
		else if (wParam == VK_UP)
			mForce->CaretToUp(), mCaret.ResetTime();
		else  if (wParam == VK_DOWN)
			mForce->CaretToDowm(), mCaret.ResetTime();
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
		mCaret.ResetTime();
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

bool InputManager::IsForce(TextInputInterface * in)
{
	return false;
}

void InputManager::SetCaretPosition(int x, int y)
{
	mCaret.SetPosition(x, y);
	mCaret.ResetTime();
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
		mCaret.Show();
		mCaret.ResetTime();
	}
	else
	{
		mCaret.Hide();
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
void InputManager::UpdateCameraMatrix(int x, int y)
{
	mCamera.UpdataSize(x, y);
}
void InputManager::Draw()
{
	WVPMatrix wvp;
	mCamera.GetCameraMatrix(wvp);
	if (mForce != nullptr)
	{
		int x, y,size;
		mForce->GetCaretProperty(x, y, size);
		mCaret.SetPosition(x, y);
		mCaret.SetHeight(size);
		mCaret.Draw(wvp);
	}
	mCursor.Draw(wvp);
}
void InputManager::Tick(float time)
{
	mCaret.Tick(time);
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
