#pragma once
#include "Action.hpp"
#include "Defines.hpp"
class RotateToAction :
	public Action
{
public:
	RotateToAction();
	~RotateToAction();
	static std::unique_ptr<RotateToAction> Make(Point & p, float time, std::shared_ptr<Interpolator> interpolator);
	virtual bool Tick(float passTime) override;
	
private:
	Point mTargetAngle;
	Point mStartAngle;
};

