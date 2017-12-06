#include "../../Include/MsgQueue.hpp"
#include <vector>
#include <ratio>
#include "../../Include/Log.hpp"
namespace XGF
{
	MsgQueue::MsgQueue()
	{
	}


	MsgQueue::~MsgQueue()
	{
		mQueue.clear();
	}

	const Event & MsgQueue::GetMsg()
	{
		std::lock_guard<std::mutex>lk(mutex);
		if (mQueue.empty()) {
			return EventPool::GetNullEvent();
		}
		const Event &msg = *mQueue.front();
		mQueue.erase(mQueue.begin());
		return msg;
	}

	void MsgQueue::InsertMsg(const Event & msg)
	{
		Event * e = const_cast<Event *>(&msg);
		std::lock_guard<std::mutex>lk(mutex);
		auto rt = std::find_if(mQueue.begin(), mQueue.end(), [e](Event * msg) {return msg->Message > e->Message;});
		mQueue.insert(rt, e);
	}
	void MsgQueue::InsertMsgWithoutRepeat(const Event & msg)
	{
		Event * e = const_cast<Event *>(&msg);
		std::lock_guard<std::mutex>lk(mutex);
		auto rt = mQueue.begin();
		for (;;) {
			rt = std::find_if(rt, mQueue.end(), [e](Event * msg) {return msg->Message == e->Message;});
			if (rt != mQueue.end())
			{
				rt = mQueue.erase(rt);
			}
			else
				break;

		}
		mQueue.insert(rt, e);
	}
	void MsgQueue::Notify()
	{
		mcVariable.notify_all();
	}
	void MsgQueue::Wait()
	{
		mcVariable.wait_for(std::unique_lock<std::mutex>(mutex), std::chrono::duration<int, std::milli>(100));
	}

};
