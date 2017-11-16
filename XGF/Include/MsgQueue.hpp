#pragma once
#include "Defines.hpp"
#include <thread>
#include <list>
#include <mutex>
#include "EventPool.hpp"
#include <algorithm>
/*
一个简单的线程安全的消息队列的实现
*/
class MsgQueue
{
public:
	MsgQueue();
	~MsgQueue();
	const Event& GetMsg();
	void InsertMsg(const Event& msg);
	void InsertMsgWithoutRepeat(const Event& msg);
	void Wait();
	void Notify();
	std::mutex* GetMutex() { return &mutex; }
private:
	std::list<Event *> mQueue;
	std::mutex mutex;
	std::condition_variable mcVariable;
};


