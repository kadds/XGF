#include "../../Include/EventPool.hpp"
#include <algorithm>
#include <time.h>
#include "../../Include/Defines.hpp"
namespace XGF
{
	Event EventPool::nullEvent;
	StateList<Event *> * EventPool::events;
	EventPool::EventPool()
	{
		nullEvent.mEventType = EventGroupType::Null;
	}

	EventPool::~EventPool()
	{

	}
	void EventPool::Initialize(unsigned int count)
	{
		events = new StateList<Event *>();
		for (unsigned i = 0; i < count; i++)
		{
			events->PushBackToA(new Event());
		}
	}
	void EventPool::Shutdown()
	{
		events->For_eachInA([](Event * e) {
			delete e;
		});
		events->For_eachInB([](Event * e) {
			delete e;
		});
		events->DelAll();
		delete events;
	}
	Event & EventPool::CreateAEvent(EventIdType id, EventGroupType evGroup, std::initializer_list<EventDataType> init)
	{
		Event *e = NewAEvent();
		e->mEventId = id;
		e->mEventType = evGroup;
		e->mMaxLiveTime = -1;
		e->mRecipteFrame = 1;
		e->mTimeStamp = time(0);
		e->mData = init;
		e->mPriority = 10;
		return *e;
	}
	Event & EventPool::CreateAEvent(EventIdType id, EventGroupType evGroup, int frameLimit, long maxLiveTime, std::initializer_list<EventDataType> init)
	{
		XGF_ASSERT(frameLimit >= 0);
		XGF_ASSERT(maxLiveTime >= 0);

		auto nowTime = time(0);
		Event *e = NewAEvent();
		e->mEventId = id;
		e->mEventType = evGroup;
		e->mMaxLiveTime = maxLiveTime + nowTime;
		e->mRecipteFrame = frameLimit;
		e->mTimeStamp = nowTime;
		e->mData = init;
		return *e;
	}
	Event& EventPool::GetNullEvent()
	{
		return nullEvent;
	}

	void EventPool::DestroyAEvent(const Event & event)
	{
		if (!IsNullEvent(event))
		{
			events->PutBToA(const_cast<Event *>(&event));
		}
	}

	Event * EventPool::NewAEvent()
	{
		Event *e;
		if (events->EmptyInA())
		{
			e = new Event();
			e->mEventType = EventGroupType::Null;
			events->PushBackToB(e);
		}
		else
		{
			e = events->GetBackInA();
			events->PutAToB();
		}
		return e;
	}

}
