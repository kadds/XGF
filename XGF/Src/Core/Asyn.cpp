#include "../../Include/Asyn.hpp"
namespace XGF {
	Asyn::Asyn()
	{
	}

	Asyn::~Asyn()
	{
	}

	void Asyn::PostEvent(EventIdType id, EventGroupType evGroup, std::initializer_list< EventDataType> init)
	{
		if (isExit) return;
		Event& ev = EventPool::CreateAEvent(id, evGroup, init);
		msgQueue.InsertMsg(ev);
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
		isExit = true;
		Event& ev = EventPool::CreateAEvent(SystemEventId::Exit, EventGroupType::System, {});
		ev.mPriority = -1;
		msgQueue.InsertMsg(ev);
	}

	void Asyn::PostWithoutRepeat(EventIdType id, std::initializer_list<EventDataType> init)
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
				if (ev.GetEventGroup() == EventGroupType::System && ev.GetSystemEventId() == SystemEventId::Exit)
				{
					EventPool::DestroyAEvent(ev);
					return true;
				}
				else
				{
					func(ev);
					EventPool::DestroyAEvent(ev);
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

	void Asyn::DestroyEvent(const Event & ev)
	{
		EventPool::DestroyAEvent(ev);
	}

}