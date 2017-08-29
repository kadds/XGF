#pragma once
#include "Defines.hpp"
#include <queue>
#include <deque>
#include <thread>
#include <mutex>
#include "EventPool.hpp"
/*
һ���򵥵��̰߳�ȫ����Ϣ���е�ʵ��
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

