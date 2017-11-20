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
#include "InputListener.hpp"

/*
* 输入管理器 基类
*/

struct MouseState
{
	long px;
	long py;
	long pz;
	int dowm;
	bool dowms[4];
	MouseState() {};

};
class DX8Input 
{
public:
	DX8Input();
	~DX8Input();
	bool Initialize(HINSTANCE hs, HWND tophwnd);//顶层窗口
	void Shutdown();
	void OnActivate(bool isActivate);
	
	// 开启新的线程，处理Input event
	void DoEvent(Asyn * asyn);

	void HandleInputMessage();

	const MouseState *GetMoustState() { return &mouseState; }
	void SetExclusiveMode(bool Exclusive);
	void SetRelativeMode(bool Relative);
	//USE DIK_?
	bool IsPress(Key key);
	void UpdateSize(int x, int y) { width = x; height = y; };

	void SetMoveable(bool m) { mMoveable = m; };
	void SetPosition(int x, int y) { mouseState.px = x; mouseState.py = y; };

private:
	void HandleMouseEvent(DIDEVICEOBJECTDATA * didod, int len, Asyn * asyn);
	void HandleKeyBoardEvent(DIDEVICEOBJECTDATA * didod, int len, Asyn * asyn);
	void UpdatePos();
	HINSTANCE hInstance;
	HWND mHwnd;
	IDirectInput* mDxInput;
	LPDIRECTINPUTDEVICE mMouse,mKeyBoard;
	Asyn mInputThread;
	DIMOUSESTATE dimouse;

	BYTE keys[256];
	MouseState mouseState;
	int width, height;
	bool mRelativeMode;
	bool mMoveable;
	DWORD id;
private:
	DISALLOW_COPY_AND_ASSIGN(DX8Input);
};

#define MOUSE_BUTTON_LEFT ( 1 << 0 )
#define MOUSE_BUTTON_RIGHT ( 1 << 1 )
#define MOUSE_BUTTON_MIDDLE ( 1 << 2 )
#define MOUSE_POSITION_RELATIVEMODE ( 1 << 3 )
#define MOUSE_POSITION_EXCLUSIVEMODE ( 1 << 4 )