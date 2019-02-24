#include "../../Include/ClickHelper.hpp"

namespace XGF
{
	ClickHelper::ClickHelper() :mState(CLICK_STATE_NORMAL)
	{
	}


	ClickHelper::~ClickHelper()
	{
	}

	void ClickHelper::OnMouseDown(const MousePoint & mp, bool isInTarget)
	{
		if ((mState == CLICK_STATE_NORMAL || mState == CLICK_STATE_FORCE) && isInTarget)
		{
			mState = CLICK_STATE_DOWN;
		}
	}

	void ClickHelper::OnMouseUp(Control * source, const MousePoint & mp, bool isInTarget)
	{
		if (mState == CLICK_STATE_DOWN)
		{
			if (isInTarget)
			{
				mState = CLICK_STATE_UP;
				OnClick(source, mp);
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
		if (mState != CLICK_STATE_DOWN)
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

	void ClickHelper::OnClick(Control * source, const MousePoint & mp)
	{
		for (auto & it : mOnClickListener)
		{
			it(source, mp,0);
		}
	}

};

