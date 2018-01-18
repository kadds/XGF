#include "../../Include/ClickHelper.hpp"

namespace XGF
{
	ClickHelper::ClickHelper() :mState(CLICK_STATE_NORMAL)
	{
	}


	ClickHelper::~ClickHelper()
	{
	}

	void ClickHelper::OnMouseDowm(const MousePoint & mp, bool isInTarget)
	{
		if ((mState == CLICK_STATE_NORMAL || mState == CLICK_STATE_FORCE) && isInTarget)
		{
			mState = CLICK_STATE_DOWM;
		}
	}

	void ClickHelper::OnMouseUp(const MousePoint & mp, bool isInTarget)
	{
		if (mState == CLICK_STATE_DOWM)
		{
			if (isInTarget)
			{
				mState = CLICK_STATE_UP;
				OnClick(mp);
				mState = CLICK_STATE_FORCE;
			}
			else
			{
				mState = CLICK_STATE_NORMAL;
			}
		}
	}

	void ClickHelper::OnMouseMove(bool isInTarget)
	{
		if (mState != CLICK_STATE_DOWM)
		{
			if (isInTarget)
			{
				mState = CLICK_STATE_FORCE;
			}
			else
			{
				mState = CLICK_STATE_NORMAL;
			}
		}

	}

	void ClickHelper::OnClick(const MousePoint & mp)
	{
		for each (auto var in mOnClickListener)
		{
			var(mp,0);
		}
	}

};

