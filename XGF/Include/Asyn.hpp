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
#include <any>

namespace XGF
{
	class GDI;
	/*
	存在在多线程下极少出现的Bug
	*/
	class Asyn
	{
	public:
		Asyn();
		~Asyn();
		

		void PostEvent(std::any id, std::initializer_list<std::any> init);

		void PostExitEvent();
		void PostWithoutRepeat(std::any id, std::initializer_list<std::any> init);
		void Wait();
		void Notify();
		void DoAsyn(std::function<void(Asyn * asyn)> fun);
		void SetCallBackFunc(std::function<void(const Event& ev)> c) { func = c; }
		bool HandleMessage();
		const Event& GetAMessage();
		void DistoryEvent(const Event& ev);
	private:
		void PostEvent(std::any id, EventGroup evGroup, std::initializer_list<std::any> init);
		std::unique_ptr<std::thread> mThread;
		MsgQueue msgQueue;
		std::function<void(const Event& ev)> func;
		bool isExit;
		DISALLOW_COPY_AND_ASSIGN(Asyn);
	};


}
