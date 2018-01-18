#include "../../Include/EventPool.hpp"
#include "../../Include/Log.hpp"
#include <algorithm>
#include <time.h>
#include "../../Include/Defines.hpp"
namespace XGF
{
	Event EventPool::nullEvent;
	StateList<Event *> * EventPool::events;
	EventPool::EventPool()
	{
		nullEvent.mEventType = EventGroup::Null;
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
	Event & EventPool::CreateAEvent(std::any id, EventGroup evGroup, std::initializer_list<std::any> init)
	{
		Event *e = NewAEvent();
		e->mEventId = id;
		e->mEventType = evGroup;
		e->mMaxLiveTime = -1;
		e->mRecipteFrame = 1;
		e->mTimeStamp = time(0);
		e->mData.clear();
		e->mData = init;
		e->mPriority = 10;
		return *e;
	}
	Event & EventPool::CreateAEvent(std::any id, EventGroup evGroup, int frameLimit, long maxLiveTime, std::initializer_list<std::any> init)
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
		e->mData.clear();
		e->mData = init;
		return *e;
	}
	Event& EventPool::GetNullEvent()
	{
		return nullEvent;
	}

	void EventPool::DistoryAEvent(const Event & event)
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
			e->mEventType = EventGroup::Null;
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
