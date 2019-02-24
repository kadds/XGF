#include "..\..\Include\InputManager.hpp"
#include "../../Include/Application.hpp"
#include "../../Include/Context.hpp"

namespace XGF
{
	InputManager::InputManager() :mFocus(nullptr), mHasSetFocus(false)
	{
	}


	InputManager::~InputManager()
	{
	}

	bool InputManager::Initialize()
	{
		if (!dinput.Initialize())
			return false;
		auto & thread = Context::Current().QueryGameThread();
		dinput.DoEvent(&thread);
		mHwnd = Context::Current().QueryGraphicsDeviceInterface().GetTopHwnd();
		mCaret.Initialize();
		mCursor.Initialize();
		mCaretPosInText = 0;
		mCamera.SetMinDistance(0.00001f);
		SetMouseMode(MouseMode::Default);
		return true;
	}

	void InputManager::Shutdown()
	{
		mCaret.Shutdown();
		mCursor.Shutdown();
		dinput.Shutdown();
	}
	// main thread
	LRESULT InputManager::ProcessInputMessage(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_MOUSEMOVE:
			{
				LRESULT type = SendMessage(mHwnd, WM_NCHITTEST, 0, lParam);
				if (type == HTCLIENT)
				{
					dinput.MakeValid();
				}
				else
				{
					dinput.MakeInvalid();
				}
			}
			break;
		case WM_KEYDOWN:
			if (mFocus == nullptr) break;
			if (wParam == VK_DELETE)
				mFocus->Delete(), mCaret.ResetTime();
			else if (wParam == VK_LEFT)
				mFocus->CaretToLeft(), mCaret.ResetTime();
			else  if (wParam == VK_RIGHT)
				mFocus->CaretToRight(), mCaret.ResetTime();
			else if (wParam == VK_UP)
				mFocus->CaretToUp(), mCaret.ResetTime();
			else  if (wParam == VK_DOWN)
				mFocus->CaretToDown(), mCaret.ResetTime();
			break;
		case WM_CHAR:
		{
			if (mFocus == nullptr) break;
			if (wParam == VK_BACK)
			{
				mFocus->BackUp();
			}
			else if (wParam == VK_ESCAPE || wParam == VK_LWIN || wParam == VK_RWIN || wParam == VK_TAB)
			{
			}
			else if (wParam == VK_RETURN)
				mFocus->AppendInputStr(L'\n');
			else
				mFocus->AppendInputStr(static_cast<wchar_t>(wParam));
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

	bool InputManager::IsFocus(TextInputProcessor * in) const
	{
		if (mFocus != nullptr)
			if (mFocus == in)
				return true;
		return false;
	}

	void InputManager::SetCaretPosition(float x, float y)
	{
		mCaret.SetPosition(x, y);
		mCaret.ResetTime();
	}


	void InputManager::ClearFocus(TextInputProcessor * tei)
	{
		auto ret = std::find(std::begin(mInputs), std::end(mInputs), tei);
		if (ret != std::end(mInputs))
			mInputs.erase(ret);
		if (mFocus == tei)
			mFocus = nullptr;
	}
	void InputManager::SetFocus(TextInputProcessor * tei)
	{
		if (tei != nullptr)
		{
			mCaret.Show();
			mCaret.ResetTime();
		}
		else
		{
			mCaret.Hide();
			if (mFocus != nullptr)
				mFocus->OnFocus(false);
			mFocus = nullptr;
			return;
		}
		auto ret = std::find(std::begin(mInputs), std::end(mInputs), tei);
		if (ret == std::end(mInputs))
			mInputs.push_back(tei);
		for (auto & it : mInputs)
		{
			if (it != tei)
			{
				it->OnFocus(false);
			}
			else
			{
				it->OnFocus(true);
			}
		}
		mFocus = tei;
		mHasSetFocus = true;
	}
	void InputManager::UpdateCameraMatrix(int x, int y)
	{
		dinput.UpdateSize(x, y);
		mCamera.UpdateProject(x, y);
		if (mMouseMode == MouseMode::CustomCenter)
		{
			mCursor.SetPosition(x / 2.f, y / 2.f);
		}
	}
	void InputManager::Draw()
	{
		WVPMatrix wvp;
		mCamera.GetCameraMatrix(wvp);
		if (mFocus != nullptr)
		{
			float x, y;
			int size;
			mFocus->GetCaretProperty(x, y, size);
			mCaret.SetPosition(x, y);
			mCaret.SetHeight(size);
			mCaret.Draw(wvp);
		}
		mCursor.Draw(wvp);
	}
	void InputManager::Tick(float time)
	{
		mCaret.Tick(time);
		mCursor.Tick(time);
	}
	void InputManager::StartForFocus()
	{
		mHasSetFocus = false;
	}
	void InputManager::StopForFocus()
	{
		if (!mHasSetFocus)
		{
			SetFocus(nullptr);
		}
	}

	Cursor & InputManager::GetCursor()
	{
		return mCursor;
	}

	void InputManager::OnMouseMove(float x, float y)
	{
		if (MouseMode::CustomCenter == mMouseMode)
		{
			return;
		}
		mCursor.SetPosition(x, y);
	}

	void InputManager::SetMouseMode(MouseMode mm)
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		auto w = gdi.GetWidth() / 2, h = gdi.GetHeight() / 2;
		switch (mm)
		{
		case MouseMode::Default:
			dinput.SetMoveable(true);
			mCursor.Hide();
			PostMessage(mHwnd, WM_X_SHOWORHIDECURSOR, TRUE, 0);
			break;
		case MouseMode::Center:
			mCursor.Hide();
			dinput.SetMoveable(false);
			dinput.SetPosition(w, h);
			dinput.SetRelativeMode(true);
			PostMessage(mHwnd, WM_X_SHOWORHIDECURSOR, TRUE, 0);
			break;
		case MouseMode::Custom:
			dinput.SetMoveable(true);
			dinput.SetRelativeMode(false);
			mCursor.Show();
			PostMessage(mHwnd, WM_X_SHOWORHIDECURSOR, FALSE, 0);
			break;
		case MouseMode::CustomCenter:
			dinput.SetMoveable(true);
			dinput.SetPosition(w, h);
			dinput.SetRelativeMode(true);
			PostMessage(mHwnd, WM_X_SHOWORHIDECURSOR, FALSE, 0);
			OnMouseMove(w, h);
			mCursor.Show();
			break;
		default:
			break;
		}
		mMouseMode = mm;
	}
	void InputManager::SetExclusiveMouseMode()
	{
		dinput.SetExclusiveMode(true);
	}

	void InputManager::SetNoExclusiveMouseMode()
	{
		dinput.SetExclusiveMode(false);
	}

	bool InputManager::IskeyDown(Key k)
	{
		return dinput.IsPress(k);
	}

}
