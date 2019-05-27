#pragma once
#include "Defines.hpp"
#include "InputDefinitions.hpp"
#include <Windows.h>

namespace XGF::Input 
{
	class RawInput 
	{
	public:
		bool Initialize();
		void Shutdown();
		void SetExclusiveMode(bool isExclusiveMode);
		long ProcessInputMessage(WPARAM wParam, LPARAM lParam);
		long ProcessInputDeviceChangedMessage(WPARAM wParam, LPARAM lParam);
		const MouseContent& GetMouseContent();
		const KeyboardContent& GetKeyboardContent();
		
	private:
		MouseContent mMouseContent;
		KeyboardContent mKeyboardContent;

		unsigned int mPauseScancodeRead;
	};
}