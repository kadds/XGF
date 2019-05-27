#pragma once
#include "Defines.hpp"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "Asyn.hpp"
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#define COOPERATIVE_LEVEL_EXCLUSIVE true
#define COOPERATIVE_LEVEL_NONEXCLUSIVE false
#define COOPERATIVE_LEVEL_FOREGROUND false
#define COOPERATIVE_LEVEL_BACKGROUND true
#include "ClickHelper.hpp"
#include "InputDefinitions.hpp"
namespace XGF::Input
{
	class DX8Input
	{
	public:
		DX8Input();
		~DX8Input();
		bool Initialize();
		void Shutdown();
		void OnActivate(bool isActivate);

		void HandleInputMessage();

		void SetExclusiveMode(bool Exclusive);

		void UpdateSize(int x, int y) { width = x; height = y; };

		void MakeInvalid() { mInvalid = true; };
		void MakeValid() { mInvalid = false; };
		bool IsInValid() { return mInvalid; };

		const MouseContent& GetMouseContent();
		const KeyboardContent& GetKeyboardContent();
	private:
		void DoEvent();

		MouseContent mMouseContent;
		KeyboardContent mKeyboardContent;

		void HandleMouseEvent(DIDEVICEOBJECTDATA * didod, int len);
		void HandleKeyBoardEvent(DIDEVICEOBJECTDATA * didod, int len);

		IDirectInput* mDxInput;

		LPDIRECTINPUTDEVICE mMouse, mKeyBoard;

		DIMOUSESTATE dimouse;

		int width, height;
		bool mInvalid;

		Asyn mInputThread;
	private:
		DisableCopyAndAssign(DX8Input);
	};
}

