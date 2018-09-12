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
		static Event& CreateAEvent(EventIdType id, EventGroupType evGroup, std::initializer_list<EventDataType>);
		//maxLiveTime:�������ʱ�䣬 ms�� ��0Ϊ��ʼ��ʱ
		static Event& CreateAEvent(EventIdType id, EventGroupType evGroup, int frameLimit, long maxLiveTime, std::initializer_list<EventDataType>);

		static Event& GetNullEvent();
		static void DistoryAEvent(const Event & event);

		inline static bool IsNullEvent(const Event & ev) { return  ev.GetEventGroup() == EventGroupType::Null; }
	private:
		EventPool();
		~EventPool();
		static Event * NewAEvent();
		static Event nullEvent;
		static StateList<Event *> * events;
	};

}
