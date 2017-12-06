#include "../../Include/ParallelAction.hpp"
namespace XGF
{

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


}


