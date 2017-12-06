#pragma once
#include "Defines.hpp"
#include "DX8Input.hpp"
#include "TextInputInterface.hpp"


#include "OrthoCamera.hpp"
#include "Caret.hpp"
#include "Cursor.hpp"
#include <vector>
namespace XGF
{
	enum MouseMode {
		Default,
		Center,
		Custom,
		CustomCenter
	};

	class InputManager
	{
	public:
		InputManager();
		~InputManager();
		bool Initialize(GDI * gdi, HINSTANCE hs, HWND hwnd, Asyn * a);//¶¥²ã´°¿Ú
		void Shutdown();
		LRESULT ProcessInputMessage(UINT msg, WPARAM wParam, LPARAM lParam);
		void OnActivate(bool isActivate);
		bool IsForce(TextInputInterface * in);
		void SetCaretPosition(float x, float y);
		void SetExclusiveMouseMode();
		void SetNoExclusiveMouseMode();
		bool IskeyDowm(Key k);
		void ClearForce(TextInputInterface * tei);
		void SetForce(TextInputInterface * tei);
		void UpdateCameraMatrix(int x, int y);
		void Draw();
		void Tick(float time);
		void StartForForce();
		void StopForForce();
		Cursor * GetCursor() { return &mCursor; };
		void ShowCursor() {
			mCursor.Show();
			SetExclusiveMouseMode();
		};
		void HideCursor() {
			mCursor.Hide();
			SetNoExclusiveMouseMode();
		};
		void OnMouseMove(float x, float y)
		{
			if (CustomCenter == mMouseMode)
			{
				return;
			}
			mCursor.SetPosition(x, y);
		}
		void SetMouseMode(MouseMode mm);

	private:
		DX8Input dinput;
		HWND mHwnd;
		TextInputInterface * mForce;
		bool mHasSetForce;
		int mCaretPosInText;
		Cursor mCursor;
		Caret mCaret;
		MouseMode mMouseMode;

		OrthoCamera mCamera;
		std::vector<TextInputInterface *> mInputs;
		DISALLOW_COPY_AND_ASSIGN(InputManager);
	};


}
