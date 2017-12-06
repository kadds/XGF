#include "../../Include/EventPool.hpp"
#include "../../Include/Log.hpp"
#include <algorithm>
#include "../../Include/Defines.hpp"
namespace XGF
{
	Event EventPool::nullEvent(0);
	StateList<Event *> * EventPool::events;
	EventPool::EventPool()
	{
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
	Event & EventPool::CreateAEvent(int msg)
	{
		Event *e = NewAEvent();
		e->Message = msg;
		return *e;
	}
	Event & EventPool::CreateAEvent(int msg, int data)
	{
		Event *e = NewAEvent();
		e->Message = msg;
		e->Content.x.num = data;
		return *e;
	}

	Event & EventPool::CreateAEvent(int msg, int data1, int data2)
	{
		Event *e = NewAEvent();
		e->Message = msg;
		e->Content.x.num = data1;
		e->Content.y.num = data2;
		return *e;
	}
	Event & EventPool::CreateAEvent(int msg, int data1, int data2, int data3)
	{
		Event *e = NewAEvent();
		e->Message = msg;
		e->Content.x.num = data1;
		e->Content.y.num = data2;
		e->Content.z.num = data3;

		return *e;
	}
	Event & EventPool::CreateAEvent(int msg, int data1, int data2, void * address)
	{
		Event *e = NewAEvent();
		e->Message = msg;
		e->Content.x.num = data1;
		e->Content.y.num = data2;
		e->Content.z.address = address;
		return *e;
	}
	Event & EventPool::CreateAEvent(int msg, int data1, void * address, void * address2)
	{
		Event *e = NewAEvent();
		e->Message = msg;
		e->Content.x.num = data1;
		e->Content.y.address = address;
		e->Content.z.address = address2;
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
			e = new Event(0);
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
