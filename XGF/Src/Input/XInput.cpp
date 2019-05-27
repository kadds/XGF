#include "../../Include/Xinput.hpp"
#include "../../Include/Logger.hpp"

#pragma comment(lib, "Xinput.lib")
namespace XGF::Input
{
	void XInput::Initialize()
	{
		ZeroMemory(&mState, sizeof(XINPUT_STATE) * XUSER_MAX_COUNT);

		DWORD dwResult;
		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			dwResult = XInputGetState(i, &mState[i].second);
			if (dwResult == ERROR_SUCCESS)
			{
				mState[i].first = true;
			}
			else 
			{
				mState[i].first = false;
			}
		}
		XGF_Error(Input, "XInput is unavailable in this version.");
	}
	void XInput::Shutdown()
	{
	}
}