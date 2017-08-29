#pragma once
#include "Timer.hpp"
#include "Defines.hpp"
#include "InputManager.hpp"
#include "InputListener.hpp"
#include "Asyn.hpp"
#include <list>

class Scene;
class GDI;
/*
这是应用程序框架
*请继承此类并实现抽象方法
*/
class XGFramework
{
public:
	XGFramework();
	~XGFramework();
	//框架调用，在该函数初始化
	virtual void OnCreate() = 0;
	//框架调用，在该函数释放资源
	virtual void OnDestory() = 0;
	//框架调用，注意更新Camera
	virtual void OnSize(int ClientX, int ClientY) = 0;
	//框架调用
	virtual void OnActivate(bool isActivate) = 0;
	//当WM_CLOSE消息触发时 框架调用， 返回true允许退出，false不允许退出
	virtual bool OnClose() = 0;
	//每帧框架调用，用于主渲染
	virtual void Render() = 0;
	//每帧框架调用，更新逻辑和camera
	virtual void Update(float time) = 0;
	bool _Update(float time);
	//Application框架调用
	void _Loop();
	//Application框架调用
	void _Loop2();
	//Application框架调用
	void _OnCreate(GDI *gdi, Asyn * asyn);
	//Application框架调用
	void _OnDestory();
	//Application框架调用
	void _OnActivate(bool isActivate);
	//Application框架调用
	void _OnSize(int ClientX, int ClientY);
	//Application框架调用
	void _OnMessage(const Event& ev);
	//框架调用，处理感兴趣的消息，系统消息均被过滤
	virtual void OnMessage(const Event& ev) = 0;
	//Application框架调用
	bool _OnInput(const Event &ev);
	//Application框架调用
	void _OnClose();
	
	void Clean(float color[]);
	void Present(bool isVsync);

	HWND GetTopHwnd();
	HINSTANCE GetInstance();
	Asyn * GetTheard() { return mTheard; }
	GDI * GetGDI();
	int GetWindowsWidth();
	int GetWindowsHeight();
	//退出循环
	void Exit(int code);
	void AddInputListener(InputListener* il);
	void RemoveInputListener(InputListener* il);

	//Application框架调用
	LRESULT OnInputMessage(UINT msg, WPARAM wParam, LPARAM lParam) { return mInputManager.ProcessInputMessage(msg, wParam, lParam); }

	InputManager * GetInputManager() { return &mInputManager; }
protected://来自外部初始化的变量
	GDI *mGDI;
protected:
	Timer mainTimer; 
	float mDeltaTime;
	InputManager mInputManager;
	std::list<InputListener* > mInputs;
	Asyn * mTheard;
private:
	DISALLOW_COPY_AND_ASSIGN(XGFramework);
};
