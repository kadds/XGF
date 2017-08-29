#include "..\..\Include\ChangeColorAction.hpp"
#include "..\..\Include\Animation.hpp"
#include "..\..\Include\Actor.hpp"


ChangeColorAction::ChangeColorAction()
{
}


ChangeColorAction::~ChangeColorAction()
{
}

std::unique_ptr<ChangeColorAction> ChangeColorAction::Make(Color & c, float time, std::shared_ptr<Interpolator> interpolator)
{
	std::unique_ptr<ChangeColorAction>  ca = std::make_unique<ChangeColorAction>();
	ca->mTargetColor = c;
	ca->mInterpolator = interpolator;
	ca->mTime = time;
	return ca;
}

bool ChangeColorAction::Tick(float passTime)
{
	if (mComplete)
	{
		return true;
	}
	if (!mBegan)
	{
		mBegan = true;
		mTarget->GetColor(mStartColor, mID);
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
		mTarget->OnColorChange(Color(mTargetColor.x*k + mStartColor.x, mTargetColor.y*k + mStartColor.y, mTargetColor.z*k + mStartColor.z, mTargetColor.w*k + mStartColor.w), mID);
	else
		mTarget->OnColorChange(Color((mTargetColor.x - mStartColor.x)*k + mStartColor.x, (mTargetColor.y - mStartColor.y)*k + mStartColor.y, 
								(mTargetColor.z - mStartColor.z)*k + mStartColor.z, (mTargetColor.w - mStartColor.w)*k + mStartColor.w), mID);
	return false;
}
