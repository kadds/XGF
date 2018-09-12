#include "../../Include/EventDispatcher.hpp"

namespace XGF 
{
	void EventDispatcher::Dispatch(const Event & ev)
	{
		for (auto it : mAllEventCallBacks)
		{
			it(ev);
		}

		switch (ev.mEventType)
		{
		case EventGroupType::System:
		{
			auto ret = mSystemEventCallBacks.equal_range(ev.GetSystemEventId());
			for (auto it = ret.first; it != ret.second; ++it)
				it->second(ev);
			break;
		}
		case EventGroupType::KeyBoard:
		{
			auto ret = mKeyBoardEventCallBacks.equal_range(ev.GetKeyBoardEventId());
			for (auto it = ret.first; it != ret.second; ++it)
				it->second(ev);
			break;
		}
		case EventGroupType::Mouse:
		{
			auto ret = mMouseEventCallBacks.equal_range(ev.GetMouseEventId());
			for (auto it = ret.first; it != ret.second; ++it)
				it->second(ev);
			break;
		}case EventGroupType::Custom:
		{
			auto ret = mCustomEventCallBacks.equal_range(ev.GetCustomEventId());
			for (auto it = ret.first; it != ret.second; ++it)
				it->second(ev);
			break;
		}
		}

	}
	void EventDispatcher::InsertSystemEventListener(SystemEventId id, EventCallBack callBack)
	{
		mSystemEventCallBacks.insert(std::make_pair(id, callBack));
	}
	void EventDispatcher::InsertKeyBoardEventListener(KeyBoardEventId id, EventCallBack callBack)
	{
		mKeyBoardEventCallBacks.insert(std::make_pair(id, callBack));
	}
	void EventDispatcher::InsertMouseEventListener(MouseEventId id, EventCallBack callBack)
	{
		mMouseEventCallBacks.insert(std::make_pair(id, callBack));
	}
	void EventDispatcher::InsertCustomEventListener(CustomEventId id, EventCallBack callBack)
	{
		mCustomEventCallBacks.insert(std::make_pair(id, callBack));
	}
	void EventDispatcher::InsertAllEventListener(EventCallBack callBack)
	{
		mAllEventCallBacks.push_back(callBack);
	}
	bool EventDispatcher::RemoveSystemEventListener(SystemEventId id, EventCallBack callBack)
	{
		bool isErase = false;
		auto ret = mSystemEventCallBacks.equal_range(id);
		for (auto it = ret.first; it != ret.second; )
		{
			if (callBack != nullptr)
			{
				if (it->second.target_type() == callBack.target_type())
				{
					it = mSystemEventCallBacks.erase(it);
					isErase = true;
				}
				else
				{
					++it;
				}
			}
			else
			{
				it = mSystemEventCallBacks.erase(it);
				isErase = true;
			}
		}
		return isErase;
	}
	bool EventDispatcher::RemoveKeyBoardEventListener(KeyBoardEventId id, EventCallBack callBack)
	{
		bool isErase = false;
		auto ret = mKeyBoardEventCallBacks.equal_range(id);
		for (auto it = ret.first; it != ret.second; )
		{
			if (callBack != nullptr)
			{
				if (it->second.target_type() == callBack.target_type())
				{
					it = mKeyBoardEventCallBacks.erase(it);
					isErase = true;
				}
				else
				{
					++it;
				}
			}
			else
			{
				it = mKeyBoardEventCallBacks.erase(it);
				isErase = true;
			}
		}
		return isErase;
	}
	bool EventDispatcher::RemoveMouseEventListener(MouseEventId id, EventCallBack callBack)
	{
		bool isErase = false;
		auto ret = mMouseEventCallBacks.equal_range(id);
		for (auto it = ret.first; it != ret.second; )
		{
			if (callBack != nullptr)
			{
				if (it->second.target_type() == callBack.target_type())
				{
					it = mMouseEventCallBacks.erase(it);
					isErase = true;
				}
				else
				{
					++it;
				}
			}
			else
			{
				it = mMouseEventCallBacks.erase(it);
				isErase = true;
			}
		}
		return isErase;
	}
	bool EventDispatcher::RemoveCustomEventListener(CustomEventId id, EventCallBack callBack)
	{
		bool isErase = false;
		auto ret = mCustomEventCallBacks.equal_range(id);
		for (auto it = ret.first; it != ret.second; )
		{
			if (callBack != nullptr)
			{
				if (it->second.target_type() == callBack.target_type())
				{
					it = mCustomEventCallBacks.erase(it);
					isErase = true;
				}
				else
				{
					++it;
				}
			}
			else
			{
				it = mCustomEventCallBacks.erase(it);
				isErase = true;
			}
		}
		return isErase;
	}
	bool EventDispatcher::RemoveAllEventListener(EventCallBack callBack)
	{
		bool isErase = false;
		if (callBack == nullptr)
		{
			mAllEventCallBacks.clear();
			isErase = true;
		}
		else
		{
			for (auto it = mAllEventCallBacks.begin(); it != mAllEventCallBacks.end();)
			{
				if (it->target_type() == callBack.target_type())
				{
					isErase = true;
					it = mAllEventCallBacks.erase(it);
				}
				else
					++it;
			}
		}
		return isErase;
	}
}