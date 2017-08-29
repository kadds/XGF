#pragma once
#include "Defines.hpp"
#include "Action.hpp"
class MoveToAction :
	public Action
{
public:
	MoveToAction();
	~MoveToAction();
	static std::unique_ptr<MoveToAction> Make(Point & p, float time, std::shared_ptr<Interpolator> interpolator);
	virtual bool Tick(float time) override;
private:
	Point mTargetPosition;
	Point mStartPosition;
};

