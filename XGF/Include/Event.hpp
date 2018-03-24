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
		//�¼�������ʱ�� ms ��
		time_t mTimeStamp;
		// �ڼ�֡�����¼�
		int mRecipteFrame;
		//������¼��㣬 Ҫ������¼�������ʱ�� ���Ϊ���� ��������Ϊ����
		time_t mMaxLiveTime;
		//���ȼ��� ԽСԽ�ߣ�
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
