#pragma once
#include "Defines.hpp"
#include "DX8Input.hpp"
#include "TextInputInterface.hpp"
#include <vector>
class InputManager
{
public:
	InputManager();
	~InputManager();
	bool Initialize(HINSTANCE hs, HWND hwnd, Asyn * a);//¶¥²ã´°¿Ú
	void Shutdown();
	LRESULT ProcessInputMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	void OnActivate(bool isActivate);

	void SetExclusiveMouseMode();
	void SetNoExclusiveMouseMode();
	bool IskeyDowm(Key k);
	void CleanForce(TextInputInterface * tei);
	void SetForce(TextInputInterface * tei);

	void StartForForce();
	void StopForForce();
private:
	DX8Input dinput;
	HWND mHwnd;
	TextInputInterface * mForce;
	bool mHasSetForce;

	std::vector<TextInputInterface *> mInputs;
	DISALLOW_COPY_AND_ASSIGN(InputManager);
};

