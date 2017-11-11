#include "../../Include/Animation.hpp"
#include "../../Include/Control.hpp"
#include "../../Include/Action.hpp"
#include "../../Include/Interpolator\Interpolator.hpp"
#include "..\..\Include\Animation.hpp"


Animation::~Animation()
{
	if (mStages != nullptr)
	{
		delete[] mStages;
		mStages = nullptr;
	}
}

void Animation::Tick(float time)
{
	passTime += time;
	if (passTime >= mFrameTime + mDelayTime)
	{
		if (mLoopTimes == -1)
		{
			passTime = 0.f;
			mNowCount = mCount - 1;
		}
		else if (mLoopTimes == 0)
		{
			passTime = 0.f;
			mNowCount++;
			if (mNowCount > mCount - 1)
			{
				mNowCount = 0;
			}
		}
		else if (mLoopTimes > 0)
		{
			passTime = 0.f;
			mNowCount++;
			if (mNowCount > mCount - 1)
			{
				if (mLoopTimes == 1)
					mLoopTimes = 0;
				mNowCount = 0;
				mLoopTimes--;
			}
		}
	}
}

void Animation::SetAnimation(int LoopTimes, float frameTime, float delayTime, Texture & t, AnimationStage stages[], int stageCount)
{
	mTexture = t;
	mStages = new AnimationStage[stageCount];
	memcpy(mStages, stages, sizeof(AnimationStage) * stageCount);
	mCount = stageCount;
	mLoopTimes = LoopTimes;
	mDelayTime = delayTime;
	mFrameTime = frameTime;
	mNowCount = 0;
}

void Animation::GetPositionInTexture(float & left, float & top, float & right, float & bottom)
{
	left = mStages[mNowCount].left;
	top = mStages[mNowCount].top;
	right = mStages[mNowCount].right;
	bottom = mStages[mNowCount].bottom;
	
}

Texture * Animation::GetTexture()
{
	return &mTexture;
}
