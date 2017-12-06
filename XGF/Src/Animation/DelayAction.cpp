#include "..\..\Include\DelayAction.hpp"

namespace XGF
{

	DelayAction::DelayAction()
	{
	}


	DelayAction::~DelayAction()
	{
	}

	std::unique_ptr<DelayAction> DelayAction::Delay(float time)
	{
		std::unique_ptr<DelayAction> action = std::make_unique<DelayAction>();
		action->mTime = time;
		return action;
	}

	bool DelayAction::Tick(float passTime)
	{
		return mTime <= passTime;
	}

}
