#pragma once
#include "Defines.hpp"
#include <queue>
#include <deque>
#include <thread>
#include <mutex>
#include "EventPool.hpp"
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
	void Wait();
	void Notify();
	std::mutex* GetMutex() { return &mutex; }
private:
	std::priority_queue<Event *, std::deque<const Event*>> mQueue;
	std::mutex mutex;
	std::condition_variable mcVariable;
};

