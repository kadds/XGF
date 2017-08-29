#include "../../Include/MsgQueue.hpp"
#include <vector>
#include <ratio>
#include "../../Include/Log.hpp"

MsgQueue::MsgQueue()
{
}


MsgQueue::~MsgQueue()
{
	while (!mQueue.empty())
	{
		mQueue.pop();
	}
}

const Event & MsgQueue::GetMsg()
{
	std::lock_guard<std::mutex>lk(mutex);
	if (mQueue.empty()) {
		return EventPool::GetNullEvent();
	}
	const Event &msg = *mQueue.top();
	mQueue.pop();
	return msg;
}

void MsgQueue::InsertMsg(const Event & msg)
{
	std::lock_guard<std::mutex>lk(mutex);
	mQueue.push(&msg);
}
void MsgQueue::Notify()
{
	mcVariable.notify_all();
}
void MsgQueue::Wait()
{
	mcVariable.wait_for(std::unique_lock<std::mutex>(mutex), std::chrono::duration<int, std::milli>(100));
}
