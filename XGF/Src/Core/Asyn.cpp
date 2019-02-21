#include "../../Include/Asyn.hpp"
#include <chrono>
#include "../../Include/Timer.hpp"
#include "../../Include/Logger.hpp"

namespace XGF {
	Asyn::Asyn()
	{
	}

	Asyn::~Asyn()
	{
	}

	void Asyn::PostEvent(EventIdType id, EventGroupType evGroup, std::initializer_list< EventDataType> init)
	{
		if (mIsExit) return;
		mMessageQueue.enqueue(Event(evGroup, id, init));
	}
	EventGroupType GetType(EventIdType t)
	{
		if (std::get_if<SystemEventId>(&t) != nullptr)
			return EventGroupType::System;
		if (std::get_if<KeyBoardEventId>(&t) != nullptr)
			return EventGroupType::KeyBoard;
		if (std::get_if<MouseEventId>(&t) != nullptr)
			return EventGroupType::Mouse;
		return EventGroupType::Custom;
	}

	void Asyn::PostEvent(EventIdType id, std::initializer_list<EventDataType> init)
	{
		PostEvent(id, GetType(id), init);
	}
	void Asyn::PostExitEvent()
	{
		mIsExit = true;
		mMessageQueue.enqueue(Event(EventGroupType::System, SystemEventId::Exit, {}));
	}

	void Asyn::Wait()
	{
		auto m = std::unique_lock<std::mutex>(mutex);
		mcVariable.wait(m);
	}

	void Asyn::Wait(unsigned long long microseconds)
	{
		auto m = std::unique_lock<std::mutex>(mutex);
		mcVariable.wait_for(m, std::chrono::microseconds(microseconds));
	}

	void Asyn::Wait(std::function<bool()> predicate)
	{
		auto m = std::unique_lock<std::mutex>(mutex);
		mcVariable.wait(m, predicate);
	}

	void Asyn::Notify()
	{
		mcVariable.notify_all();
	}

	void Asyn::DoAsyn(std::function<void(Asyn * asyn)> fun)
	{
		mThread = std::make_unique<std::thread>(fun, this);
		mThread->detach();
	}

	bool Asyn::HandleMessage()
	{
		Event ev;
		Timer timer;
		float dt = 0;
		while(mMessageQueue.try_dequeue(ev))
		{
			if (ev.GetEventGroup() == EventGroupType::System && ev.GetSystemEventId() == SystemEventId::Exit)
			{
				return true;
			}
			else
			{
				mHandleCallback(ev);
			}
			dt += timer.Tick();
			if (dt > 0.001f)
			{
				break;
				//XGF_Info(Framework, "Handle event time is too long ", time * 1000, "ms");
			}
		}
		
		return false;
	}

	void Asyn::Sleep(unsigned long long microseconds)
	{
		auto d = std::chrono::microseconds(microseconds);
		std::this_thread::sleep_for(d);
	}
}
