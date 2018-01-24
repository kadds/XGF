#include "../../Include/Action.hpp"


namespace XGF
{
	Action::Action():mTime(0.f)
	{
	}


	Action::~Action()
	{
	}

	bool Action::Tick(Point * out, float deltaTime)
	{
		bool ret = false;
		mTime += deltaTime;
		if (mTime > mDuration)
		{
			mTime = mDuration;
			ret = true;
		}
		float k = mTime / mDuration;
		k = mInterpolator->Calculate(k);
		out->x = mFrom.x + mFromDelta.x * k;
		out->y = mFrom.y + mFromDelta.y * k;
		out->z = mFrom.z + mFromDelta.z * k;
		return ret;
	}

	void Action::SetFrom(Point & from)
	{
		if (mIsRelative) mFromDelta = mTo;
		else
		{
			mFromDelta.x = mTo.x - from.x;
			mFromDelta.y = mTo.y - from.y;
			mFromDelta.z = mTo.z - from.z;
		}
		mFrom = from;
	}



}
