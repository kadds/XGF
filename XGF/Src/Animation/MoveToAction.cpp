#include "../../Include/MoveToAction.hpp"
#include "../../Include/Actor.hpp"
MoveToAction::MoveToAction()
{
}


MoveToAction::~MoveToAction()
{
}

std::unique_ptr<MoveToAction> MoveToAction::Make(Point & p, float time, std::shared_ptr<Interpolator> interpolator)
{
	auto action = std::make_unique<MoveToAction>();
	action->mTargetPosition = p;
	action->mTime = time;
	action->mInterpolator = interpolator;
	return action;
}

bool MoveToAction::Tick(float passTime)
{
	if (mComplete)
	{
		return true;
	}
	if (!mBegan)
	{
		mBegan = true;
		mTarget->GetPosition(mStartPosition, mID);
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
		mTarget->OnPositionChange(Point(mTargetPosition.x*k + mStartPosition.x, mTargetPosition.y*k + mStartPosition.y
								, mTargetPosition.z*k + mStartPosition.z), mID);
	else
		mTarget->OnPositionChange(Point((mTargetPosition.x - mStartPosition.x)*k + mStartPosition.x,
								(mTargetPosition.y - mStartPosition.y)*k + mStartPosition.y,
								(mTargetPosition.z - mStartPosition.z)*k + mStartPosition.z), mID);
	return false;
}

