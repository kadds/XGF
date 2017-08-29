#include "../../Include/Asyn.hpp"
#include "../../Include/Log.hpp"
#include <Windows.h>

Asyn::Asyn()
{
}

Asyn::~Asyn()
{	
}
void Asyn::PostEvent(UINT msg, int data1, int data2, int data3)
{
	if (isExit) return;
	Event& ev = EventPool::CreateAEvent(msg, data1, data2, data3);
	msgQueue.InsertMsg(ev);
}
void Asyn::PostEvent(UINT msg, int data1, void * address, void * address2)
{
	if (isExit) return;
	Event& ev = EventPool::CreateAEvent(msg, data1, address, address2);
	msgQueue.InsertMsg(ev);
}
void Asyn::PostEvent(UINT msg, int data1, int data2, void * address)
{
	if (isExit) return;
	Event& ev = EventPool::CreateAEvent(msg, data1, data2, address);
	msgQueue.InsertMsg(ev);
}
void Asyn::PostExitEvent()
{
	DebugOut("Post Exit Event!\n")
	isExit = true;
	Event& ev = EventPool::CreateAEvent(EVENT_EXIT);
	msgQueue.InsertMsg(ev);
	
}
void Asyn::PostEvent(UINT msg, int data1, int data2)
{
	if (isExit) return;
	Event& ev = EventPool::CreateAEvent(msg, data1, data2);
	msgQueue.InsertMsg(ev);
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
	mThread = std::make_unique<std::thread>(fun,this);
	mThread->detach();
}

bool Asyn::HandleMessage()
{
	for(;;)
	{
		const Event &ev = msgQueue.GetMsg();
		if (!EventPool::IsNullEvent(ev))
			if (ev.Message == EVENT_EXIT)
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
