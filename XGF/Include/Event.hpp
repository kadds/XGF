#pragma once
#include "Defines.hpp"
#include <vector>
#include <any>
#include "Log.hpp"
namespace XGF {
	enum class EventGroup
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
		std::any mEventId;
		EventGroup mEventType;
		std::vector<std::any> mData;
	public:
		SystemEventId GetSystemEventId() const;
		KeyBoardEventId GetKeyBoardEventId() const;
		MouseEventId GetMouseEventId() const;
		EventGroup GetEventGroup() const
		{
			return mEventType;
		}
		CustomEventId GetCustomEventId() const
		{
			return std::any_cast<CustomEventId>(mEventId);
		}

		int GetDataInt(unsigned int index) const;
		template<typename Format>
		Format GetData(unsigned int index) const
		{
			XGF_ASSERT(index < mData.size());
			XGF_ASSERT(index >= 0);
			return std::any_cast<Format>(mData[index]);
		};
		float GetDataFloat(unsigned int index) const;

		unsigned int GetDataUint(unsigned int index) const;
	private:
	};
	
}
