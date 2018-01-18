#pragma once
#include "Defines.hpp"
#include "Event.hpp"
#include <unordered_map>
#include <map>
#include <functional>
namespace XGF 
{
	typedef std::function<void(const Event &)> EventCallBack;
	class EventDispatcher
	{
	public:
		void Dispatch(const Event & ev);

		void InsertSystemEventListener(SystemEventId id, EventCallBack callBack);
		void InsertKeyBoardEventListener(KeyBoardEventId id, EventCallBack callBack);
		void InsertMouseEventListener(MouseEventId id, EventCallBack callBack);
		void InsertCustomEventListener(CustomEventId id, EventCallBack callBack);

		void InsertAllEventListener(EventCallBack callBack);

		bool RemoveSystemEventListener(SystemEventId id, EventCallBack callBack = nullptr);
		bool RemoveKeyBoardEventListener(KeyBoardEventId id, EventCallBack callBack = nullptr);
		bool RemoveMouseEventListener(MouseEventId id, EventCallBack callBack = nullptr);
		bool RemoveCustomEventListener(CustomEventId id, EventCallBack callBack = nullptr);

		bool RemoveAllEventListener(EventCallBack callBack = nullptr);

	private:

		std::multimap<SystemEventId, EventCallBack> mSystemEventCallBacks;
		std::multimap<MouseEventId, EventCallBack> mMouseEventCallBacks;
		std::multimap<KeyBoardEventId, EventCallBack> mKeyBoardEventCallBacks;
		std::multimap<CustomEventId, EventCallBack> mCustomEventCallBacks;

		std::vector<EventCallBack> mAllEventCallBacks;

	};
}