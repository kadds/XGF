#pragma once
#include "Defines.hpp"
#include "StateList.hpp"
#include "Event.hpp"
#include <memory>
#include <any>

namespace XGF
{
	class EventPool
	{
	public:
		static void Initialize(unsigned int count);
		static void Shutdown();
		static Event& CreateAEvent(std::any id, EventGroup evGroup, std::initializer_list<std::any>);
		//maxLiveTime:最大生存时间， ms计 以0为开始计时
		static Event& CreateAEvent(std::any id, EventGroup evGroup, int frameLimit, long maxLiveTime, std::initializer_list<std::any>);

		static Event& GetNullEvent();
		static void DistoryAEvent(const Event & event);

		inline static bool IsNullEvent(const Event & ev) { return  ev.GetEventGroup() == EventGroup::Null; }
	private:
		EventPool();
		~EventPool();
		static Event * NewAEvent();
		static Event nullEvent;
		static StateList<Event *> * events;
	};

}
