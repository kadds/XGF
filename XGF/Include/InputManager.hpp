#pragma once
#include "Defines.hpp"
#include "DX8Input.hpp"
#include "TextInputProcessor.hpp"
#include "ClickHelper.hpp"
#include "OrthoCamera.hpp"
#include "Caret.hpp"
#include "Cursor.hpp"
namespace XGF
{
	enum class MouseMode {
		Default,
		Center,
		Custom,
		CustomCenter
	};
	class Context;
	class InputManager
	{
	public:
		InputManager();
		~InputManager();
		bool Initialize();
		InputManager(const InputManager&) = delete;
		InputManager & operator = (const InputManager &) = delete;
		void Shutdown();
		LRESULT ProcessInputMessage(UINT msg, WPARAM wParam, LPARAM lParam);
		void OnActivate(bool isActivate);
		bool IsFocus(TextInputProcessor * in) const;
		void SetCaretPosition(float x, float y);
		void SetExclusiveMouseMode();
		void SetNoExclusiveMouseMode();
		bool IskeyDown(Key k);
		void ClearFocus(TextInputProcessor * tei);
		void SetFocus(TextInputProcessor * tei);
		void UpdateCameraMatrix(int x, int y);
		void Draw();
		void Tick(float time);
		void StartForFocus();
		void StopForFocus();
		Cursor & GetCursor();
		void ShowCursor() {
			mCursor.Show();
			SetExclusiveMouseMode();
		};
		void HideCursor() {
			mCursor.Hide();
			SetNoExclusiveMouseMode();
		};
		void OnMouseMove(float x, float y);
		void SetMouseMode(MouseMode mm);

	private:
		DX8Input dinput;
		HWND mHwnd;
		TextInputProcessor * mFocus;
		bool mHasSetFocus;
		int mCaretPosInText;
		Cursor mCursor;
		Caret mCaret;
		MouseMode mMouseMode;

		OrthoCamera mCamera;
		std::vector<TextInputProcessor *> mInputs;
	};


}
