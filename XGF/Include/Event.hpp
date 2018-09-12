#pragma once
#include "Defines.hpp"
#include <variant>
#include <vector>
#include <any>

namespace XGF {
	enum class EventGroupType
	{
		Null,
		System,
		Mouse,
		KeyBoard,
		Custom,
	};
	enum class SystemEventId
	{
		Null,
		Exit,
		Size,
		Close,
		Activate,
		SwitchScene,
		AsynReport,
		AsynFinish,
	};
	enum class MouseEventId
	{
		MouseMove,
		MouseDown,
		MouseUp,
		MouseWheel,
	};
	enum class KeyBoardEventId
	{
		KeyDown,
		KeyUp,
	};
	typedef int CustomEventId;

	typedef std::variant<CustomEventId, SystemEventId, MouseEventId, KeyBoardEventId> EventIdType;
	typedef std::any EventDataType;

	class Event {
	public:
		//事件发生的时间 ms 计
		time_t mTimeStamp;
		// 第几帧接收事件
		int mRecipteFrame;
		//最长生存事件点， 要求大于事件发生的时间 如果为无限 ，则设置为负数
		time_t mMaxLiveTime;
		//优先级， 越小越高，
		int mPriority;
		
		EventIdType mEventId;
		EventGroupType mEventType;
		std::vector<EventDataType> mData;
	public:
		SystemEventId GetSystemEventId() const
		{
			return std::get<SystemEventId>(mEventId);
		};
		KeyBoardEventId GetKeyBoardEventId() const
		{
			return std::get<KeyBoardEventId>(mEventId);
		};
		MouseEventId GetMouseEventId() const
		{
			return std::get<MouseEventId>(mEventId);
		};
		EventGroupType GetEventGroup() const
		{
			return mEventType;
		}
		CustomEventId GetCustomEventId() const
		{
			return std::get<CustomEventId>(mEventId);
		}
		template<typename Format>
		Format GetData(unsigned int index) const
		{
			XGF_ASSERT(index < mData.size());
			XGF_ASSERT(index >= 0);
			return std::any_cast<Format>(mData[index]);
		}
		int GetDataInt(unsigned int index) const
		{
			return GetData<int>(index);
		}
		float GetDataFloat(unsigned int index) const
		{
			return GetData<float>(index);
		}
		unsigned GetDataUInt(unsigned int index) const
		{
			return GetData<unsigned>(index);
		}
		template<typename T>
		std::shared_ptr<T> GetDataSmartPointer(unsigned int index) const
		{
			return GetData<std::shared_ptr<T>>((index));
		}
	private:
	};
	
	
}
