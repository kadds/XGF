#include "../../Include/ScaleToAction.hpp"
#include "../../Include/Actor.hpp"

ScaleToAction::ScaleToAction()
{
}


ScaleToAction::~ScaleToAction()
{
}

std::unique_ptr<ScaleToAction> ScaleToAction::Make(Point & scale, float time, std::shared_ptr<Interpolator> interpolator)
{
	std::unique_ptr<ScaleToAction> action = std::make_unique<ScaleToAction>();
	action->mTargetScale = scale;
	action->mTime = time;
	action->mInterpolator = interpolator;
	return action;
}

bool ScaleToAction::Tick(float passTime)
{
	if (mComplete)
	{
		return true;
	}
	if (!mBegan)
	{
		mBegan = true;
		mTarget->GetScale(mStartScale, mID);
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
		mTarget->OnScaleChange(Point(mTargetScale.x*k + mStartScale.x, mTargetScale.y*k + mStartScale.y,
								mTargetScale.z*k + mStartScale.z), mID);
	else
		mTarget->OnScaleChange(Point((mTargetScale.x - mStartScale.x)*k + mStartScale.x, (mTargetScale.y - mStartScale.y)*k
								+ mStartScale.y, (mTargetScale.z - mStartScale.z)*k + mStartScale.z), mID);
	return false;
}
