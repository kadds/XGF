#pragma once
#include "StateList.hpp"
#include <memory>

#define EVENT_NULL 0
#define EVENT_EXIT 1
#define EVENT_ONINPUT 2

#define EVENT_ONSIZE 5
#define EVENT_SWITCHSCENE 7
#define EVENT_ONACTIVATE 24
#define EVENT_ONCLOSE 100

#define EVENT_ONMOUSEMOVE 565
#define EVENT_ONMOUSEDOWM 561
#define EVENT_ONMOUSEUP 562
#define EVENT_ONKEYDOWM 563
#define EVENT_ONKEYUP 564

#define EVENT_ONASYNCTASKREPORT 770
#define EVENT_ONASYNCTASKFINISH 771

#define BUTTON_LEFT 1
#define BUTTON_RIGHT 2
#define BUTTON_MIDDLE 4

#define EVENT_X 1001 
struct GMousePoint 
{
	int x;
	int y;
	int z;
	GMousePoint() {}
	GMousePoint(int v) :x(v) {}
	GMousePoint(int v, int c) :x(v), y(c) {}
	GMousePoint(int v, int c, int p) :x(v), y(c), z(p) {}
};
union AnyEx
{
	int num;
	void * address;
	bool boolvalue;
	//long longnum;
};
struct MsgContent
{
	AnyEx x;
	AnyEx y;
	AnyEx z;
};

/*
缓存大量事件对象
*/
class Event
{
public:
	int Message;
	MsgContent Content;
	Event(int msg, MsgContent c)
	{
		Message = msg;
		Content = c;
	}
	Event(int msg)
	{
		Message = msg;
		Content.x.num = 0;
	}
	Event() {}
};

class EventPool
{
public:
	static void Initialize(unsigned int count);
	static void Shutdown();
	static Event& CreateAEvent(int msg);
	static Event& CreateAEvent(int msg , int data);
	static Event& CreateAEvent(int msg, int data1, int data2);
	static  Event& CreateAEvent(int msg, int data1, int  data2, int data3);
	static  Event& CreateAEvent(int msg, int data1, int  data2, void * address);
	static  Event& CreateAEvent(int msg, int data1, void * address, void * address2);
	static Event& GetNullEvent();
	static void DistoryAEvent(const Event & event);
	
	inline static bool IsNullEvent(const Event & ev) {return  ev.Message == EVENT_NULL;}
private:
	EventPool();
	~EventPool();
	static Event * NewAEvent();
	static Event nullEvent;
	static StateList<Event *> * events;
};
