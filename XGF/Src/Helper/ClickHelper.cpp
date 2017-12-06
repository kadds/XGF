#include "../../Include/ClickHelper.hpp"

namespace XGF
{
	ClickHelper::ClickHelper() :mState(CLICK_STATE_NORMAL)
	{
	}


	ClickHelper::~ClickHelper()
	{
	}

	void ClickHelper::OnMouseDowm(const MousePoint & mp, int c, bool isIn)
	{
		if ((mState == CLICK_STATE_NORMAL || mState == CLICK_STATE_FORCE) && isIn)
		{
			mState = CLICK_STATE_DOWM;
		}
	}

	void ClickHelper::OnMouseUp(const MousePoint & mp, int c, bool isIn)
	{
		if (mState == CLICK_STATE_DOWM)
		{
			if (isIn)
			{
				mState = CLICK_STATE_UP;
				OnClick(mp, c);
				mState = CLICK_STATE_FORCE;
			}
			else
			{
				mState = CLICK_STATE_NORMAL;
			}
		}
	}

	void ClickHelper::OnMouseMove(bool isIn)
	{
		if (mState != CLICK_STATE_DOWM)
		{
			if (isIn)
			{
				mState = CLICK_STATE_FORCE;
			}
			else
			{
				mState = CLICK_STATE_NORMAL;
			}
		}

	}

	void ClickHelper::OnClick(const MousePoint & mp, int c)
	{
		for each (auto var in mOnClickListener)
		{
			var(mp, c);
		}
	}

};

