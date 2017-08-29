#include "../../Include/RotateToAction.hpp"
#include "../../Include/Actor.hpp"

RotateToAction::RotateToAction()
{
}


RotateToAction::~RotateToAction()
{
}

std::unique_ptr<RotateToAction> RotateToAction::Make(Point & p ,float time, std::shared_ptr<Interpolator> interpolator)
{
	std::unique_ptr<RotateToAction> action = std::make_unique<RotateToAction>();
	action->mTargetAngle = p;
	action->mTime = time;
	action->mInterpolator = interpolator;
	return action;
}

bool RotateToAction::Tick(float passTime)
{
	if (mComplete)
	{
		return true;
	}
	if (!mBegan)
	{
		mBegan = true;
		mTarget->GetAngle(mStartAngle, mID);
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
		mTarget->OnAngleChange(Point(mTargetAngle.x*k + mStartAngle.x, mTargetAngle.y*k + mStartAngle.y, 
			mTargetAngle.z*k + mStartAngle.z), mID);
	else
		mTarget->OnAngleChange(Point((mTargetAngle.x - mStartAngle.x)*k + mStartAngle.x, (mTargetAngle.y - mStartAngle.y)*k 
								+ mStartAngle.y, (mTargetAngle.z - mStartAngle.z)*k + mStartAngle.z), mID);
	return false;
}

