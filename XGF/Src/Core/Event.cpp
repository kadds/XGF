#include "../../Include/Event.hpp"

namespace XGF {
	SystemEventId Event::GetSystemEventId() const
	{
		XGF_ASSERT(mEventId.type() == typeid(SystemEventId));
		return std::any_cast<SystemEventId>(mEventId);
	}
	KeyBoardEventId Event::GetKeyBoardEventId() const
	{
		XGF_ASSERT(mEventId.type() == typeid(KeyBoardEventId));
		return std::any_cast<KeyBoardEventId>(mEventId);
	}
	MouseEventId Event::GetMouseEventId() const
	{
		XGF_ASSERT(mEventId.type() == typeid(MouseEventId));
		return std::any_cast<MouseEventId>(mEventId);
	}
	int Event::GetDataInt(unsigned int index) const
	{
		XGF_ASSERT(index < mData.size());
		XGF_ASSERT(index >= 0);
		return std::any_cast<int>(mData[index]);
	}
	float Event::GetDataFloat(unsigned int index) const
	{
		XGF_ASSERT(index < mData.size());
		XGF_ASSERT(index >= 0);
		return std::any_cast<float>(mData[index]);
	}
	unsigned int Event::GetDataUint(unsigned int index) const
	{
		XGF_ASSERT(index < mData.size());
		XGF_ASSERT(index >= 0);
		return std::any_cast<unsigned int>(mData[index]);
	}
}
