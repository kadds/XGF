#pragma once
#include "Defines.hpp"
#include <thread>
#include <functional>
#include <Windows.h>
#include "MsgQueue.hpp"
#include <condition_variable> 
#include "EventPool.hpp"
#include <memory>
#include <future>
class GDI;
/*
存在在多线程下极少出现的Bug
*/
class Asyn
{
public:
	Asyn();
	~Asyn();
	void PostEvent(UINT msg, int data1, int data2, int data3);
	void PostEvent(UINT msg, int data1, void * address, void * address2);
	void PostEvent(UINT msg, int data1, int data2, void * address);
	void PostExitEvent();
	void PostEvent(UINT msg, int data1, int data2);
	void PostWithoutRepeat(UINT msg, int data1, int data2);
	void Wait();
	void Notify();
	void DoAsyn(std::function<void(Asyn * asyn)> fun);
	void SetCallBackFunc(std::function<void(const Event& ev)> c) { func = c; }
	bool HandleMessage();
	const Event& GetAMessage();
	void DistoryEvent(const Event& ev);
private:
	std::unique_ptr<std::thread> mThread;
	MsgQueue msgQueue;
	std::function<void(const Event& ev)> func;
	bool isExit;
	DISALLOW_COPY_AND_ASSIGN(Asyn);
};

