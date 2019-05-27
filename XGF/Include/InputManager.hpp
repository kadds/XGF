#pragma once
#include "Defines.hpp"
#include "DX8Input.hpp"
#include "Xinput.hpp"
#include "RawInput.hpp"

#include "TextInputProcessor.hpp"
#include "ClickHelper.hpp"
#include "OrthoCamera.hpp"

namespace XGF 
{
	class Context;
}
namespace XGF::Input
{
	
	class InputManager
	{
	public:
		InputManager();
		~InputManager();
		void Initialize();
		void Shutdown();
		void Tick(float delta);

		LRESULT ProcessInputMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void OnActivate(bool isActivate);

		void UpdateCameraMatrix(int x, int y);

		void SetMouseMode(MouseMode mm);

		void EnableXInput();
		void DisableXInput();

		bool SwitchToRawInput();
		bool SwitchToDInput();
		bool IsRawInput();
		bool IsDInput();
		bool IsXInputEnable();
		const MouseContent& GetMouseContent();
		const KeyboardContent& GetKeyboardContent();

		bool IskeyDown(MouseKey k);
		bool IskeyDown(KeyBoardKey k);
		
		bool IsRelativeMouse() const;
		void SetRelativeMouse(bool isRelativeMouse);


	private:
		std::unique_ptr<DX8Input> dinput;
		std::unique_ptr<XInput> xinput;
		std::unique_ptr<RawInput> rawinput;

		MouseContent mCurrentMouseContent;
		KeyboardContent mCurrentKeyBoardContent;

		MouseMode mMouseMode;
		bool mIsRelativeMouse;

		DisableCopyAndAssign(InputManager);
	};


}
