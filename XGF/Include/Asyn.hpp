#pragma once
#include "Defines.hpp"
#include <thread>
#include <functional>
#ifdef new
#undef new
#endif
#include "Concurrentqueue.hpp"
#include "Event.hpp"
#include <mutex>
#ifdef _XGF_DEBUG_ALLOC
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)  
#endif


namespace XGF
{
	class Asyn
	{
	public:
		Asyn();
		~Asyn();
		

		void PostEvent(EventIdType id, std::initializer_list<EventDataType> init);

		void PostExitEvent();
		void Wait();
		void Wait(unsigned long long microseconds);
		void Wait(std::function<bool()> predicate);
		void Notify();
		void DoAsyn(std::function<void(Asyn * asyn)> fun);
		void SetCallBackFunc(std::function<void(const Event& ev)> callback) { mHandleCallback = callback; }
		bool HandleMessage();
		void Sleep(unsigned long long microseconds);
	private:
		void PostEvent(EventIdType id, EventGroupType evGroup, std::initializer_list<EventDataType> init);
		std::unique_ptr<std::thread> mThread;
		moodycamel::ConcurrentQueue<Event> mMessageQueue;
		std::function<void(const Event& ev)> mHandleCallback;
		bool mIsExit;
		std::mutex mutex;
		std::condition_variable mcVariable;
		DisableCopyAndAssign(Asyn);
	};


}
