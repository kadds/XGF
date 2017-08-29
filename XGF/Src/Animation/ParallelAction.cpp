#include "../../Include/ParallelAction.hpp"



ParallelAction::ParallelAction()
{
}


ParallelAction::~ParallelAction()
{
	
}

bool ParallelAction::Tick(float time)
{
	bool isEnd = true;
	std::for_each(actions.begin(), actions.end(), [&](std::unique_ptr<Action> &var) {
		if (!var->Tick(time))
			isEnd = false;
	});
	return isEnd;
}


