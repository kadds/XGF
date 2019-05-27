#pragma once
#include "Defines.hpp"
#include <Xinput.h>
#include <vector>

namespace XGF::Input 
{
	class XInput 
	{
	public:
		void Initialize();
		void Shutdown();
	private:
		std::pair<bool, XINPUT_STATE> mState[XUSER_MAX_COUNT];
	};
}
