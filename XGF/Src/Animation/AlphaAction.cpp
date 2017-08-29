#include "..\..\Include\AlphaAction.hpp"



AlphaAction::AlphaAction()
{
}


AlphaAction::~AlphaAction()
{
	
}

std::unique_ptr<AlphaAction> AlphaAction::Make(float c, float time, std::shared_ptr<Interpolator> interpolator)
{
	auto ca = std::make_unique<AlphaAction>();
	ca->mTargetAlpha = c;
	ca->mInterpolator = interpolator;
	ca->mTime = time;
	return ca;
}

bool AlphaAction::Tick(float passTime)
{
	if (mComplete)
	{
		return true;
	}
	if (!mBegan)
	{
		mBegan = true;
		mTarget->GetAlpha(mStartAlpha, mID);
	}
	float k;
	if (passTime > mTime)
	{
		k = 1;
		mComplete = true;
	}
	else
		k = mInterpolator->Calculate(passTime / mTime);
	if (mIsRelativeMode)
		mTarget->OnAlphaChange(mTargetAlpha*k + mStartAlpha, mID);
	else
		mTarget->OnAlphaChange((mTargetAlpha - mStartAlpha)*k + mStartAlpha, mID);
	return false;
}
