#pragma once
#include "Defines.hpp"
#include <list>
#include <mutex>
#include "EventPool.hpp"
namespace XGF
{
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
		void InsertMsgWithoutRepeat(const Event& msg);
		void Wait();
		void Notify();
		std::mutex* GetMutex() { return &mutex; }
	private:
		std::list<Event *> mQueue;
		std::mutex mutex;
		std::condition_variable mcVariable;
	};
};



