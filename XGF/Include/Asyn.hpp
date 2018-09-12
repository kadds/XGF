#pragma once
#include "Defines.hpp"
#include <thread>
#include <functional>
#include "MsgQueue.hpp"
#include "EventPool.hpp"
#include <memory>

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
		

		void PostEvent(EventIdType id, std::initializer_list<EventDataType> init);

		void PostExitEvent();
		void PostWithoutRepeat(EventIdType id, std::initializer_list<EventDataType> init);
		void Wait();
		void Notify();
		void DoAsyn(std::function<void(Asyn * asyn)> fun);
		void SetCallBackFunc(std::function<void(const Event& ev)> c) { func = c; }
		bool HandleMessage();
		const Event& GetAMessage();
		void DistoryEvent(const Event& ev);
	private:
		void PostEvent(EventIdType id, EventGroupType evGroup, std::initializer_list<EventDataType> init);
		std::unique_ptr<std::thread> mThread;
		MsgQueue msgQueue;
		std::function<void(const Event& ev)> func;
		bool isExit;
		DISALLOW_COPY_AND_ASSIGN(Asyn);
	};


}
