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
����Ӧ�ó�����
*��̳д��ಢʵ�ֳ��󷽷�
*/
class XGFramework
{
public:
	XGFramework();
	~XGFramework();
	//��ܵ��ã��ڸú�����ʼ��
	virtual void OnCreate() = 0;
	//��ܵ��ã��ڸú����ͷ���Դ
	virtual void OnDestory() = 0;
	//��ܵ��ã�ע�����Camera
	virtual void OnSize(int ClientX, int ClientY) = 0;
	//��ܵ���
	virtual void OnActivate(bool isActivate) = 0;
	//��WM_CLOSE��Ϣ����ʱ ��ܵ��ã� ����true�����˳���false�������˳�
	virtual bool OnClose() = 0;
	//ÿ֡��ܵ��ã���������Ⱦ
	virtual void Render() = 0;
	//ÿ֡��ܵ��ã������߼���camera
	virtual void Update(float time) = 0;
	bool _Update(float time);
	//Application��ܵ���
	void _Loop();
	//Application��ܵ���
	void _Loop2();
	//Application��ܵ���
	void _OnCreate(GDI *gdi, Asyn * asyn);
	//Application��ܵ���
	void _OnDestory();
	//Application��ܵ���
	void _OnActivate(bool isActivate);
	//Application��ܵ���
	void _OnSize(int ClientX, int ClientY);
	//Application��ܵ���
	void _OnMessage(const Event& ev);
	//��ܵ��ã��������Ȥ����Ϣ��ϵͳ��Ϣ��������
	virtual void OnMessage(const Event& ev) = 0;
	//Application��ܵ���
	bool _OnInput(const Event &ev);
	//Application��ܵ���
	void _OnClose();
	
	void Clean(float color[]);
	void Present(bool isVsync);

	HWND GetTopHwnd();
	HINSTANCE GetInstance();
	Asyn * GetTheard() { return mTheard; }
	GDI * GetGDI();
	int GetWindowsWidth();
	int GetWindowsHeight();
	//�˳�ѭ��
	void Exit(int code);
	void AddInputListener(InputListener* il);
	void RemoveInputListener(InputListener* il);

	//Application��ܵ���
	LRESULT OnInputMessage(UINT msg, WPARAM wParam, LPARAM lParam) { return mInputManager.ProcessInputMessage(msg, wParam, lParam); }

	InputManager * GetInputManager() { return &mInputManager; }
protected://�����ⲿ��ʼ���ı���
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
