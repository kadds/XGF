#include "../../Include/Asyn.hpp"
#include "../../Include/Log.hpp"
#include <Windows.h>
namespace XGF {
	Asyn::Asyn()
	{
	}

	Asyn::~Asyn()
	{
	}

	void Asyn::PostEvent(std::any id, EventGroup evGroup, std::initializer_list<std::any> init)
	{
		if (isExit) return;
		Event& ev = EventPool::CreateAEvent(id, evGroup, init);
		msgQueue.InsertMsg(ev);
	}
	 template<typename T>
	inline constexpr EventGroup GetType(T t)
	{
		if (t.type() == typeid(SystemEventId))
			return EventGroup::System;
		if (t.type() == typeid(KeyBoardEventId))
			return EventGroup::KeyBoard;
		if (t.type() == typeid(MouseEventId))
			return EventGroup::Mouse;
		return EventGroup::Custom;
	}
	
	void Asyn::PostEvent(std::any id, std::initializer_list<std::any> init)
	{
		PostEvent(id, GetType(id), init);
	}
	void Asyn::PostExitEvent()
	{
		isExit = true;
		Event& ev = EventPool::CreateAEvent(SystemEventId::Exit, EventGroup::System, {});
		ev.mPriority = -1;
		msgQueue.InsertMsg(ev);
	}

	void Asyn::PostWithoutRepeat(std::any id, std::initializer_list<std::any> init)
	{
		Event& ev = EventPool::CreateAEvent(id, GetType(id), init);
		msgQueue.InsertMsgWithoutRepeat(ev);
	}

	void Asyn::Wait()
	{
		msgQueue.Wait();
	}

	void Asyn::Notify()
	{
		msgQueue.Notify();
	}

	void Asyn::DoAsyn(std::function<void(Asyn * asyn)> fun)
	{
		mThread = std::make_unique<std::thread>(fun, this);
		mThread->detach();
	}

	bool Asyn::HandleMessage()
	{
		for (;;)
		{
			const Event &ev = msgQueue.GetMsg();
			if (!EventPool::IsNullEvent(ev))
				if (ev.GetEventGroup() == EventGroup::System && ev.GetSystemEventId() == SystemEventId::Exit)
				{
					EventPool::DistoryAEvent(ev);
					return true;
				}
				else
				{
					func(ev);
					EventPool::DistoryAEvent(ev);
				}
			else
				break;
		}
		return false;
	}

	const Event & Asyn::GetAMessage()
	{
		return msgQueue.GetMsg();
	}

	void Asyn::DistoryEvent(const Event & ev)
	{
		EventPool::DistoryAEvent(ev);
	}

}