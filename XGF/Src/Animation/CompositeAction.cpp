#include "..\..\Include\CompositeAction.hpp"


namespace XGF
{
	CompositeAction::CompositeAction() :parent(nullptr)
	{
	}


	CompositeAction::~CompositeAction()
	{

	}
	void CompositeAction::AddAction(std::unique_ptr<Action> action)
	{
		actions.push_back(std::move(action));
	}

	void CompositeAction::Reset()
	{
		std::for_each(actions.begin(), actions.end(), [](std::unique_ptr<Action> &var) {
			var->Reset();
		});
	}

	void CompositeAction::SetTarget(ActionInterface * target)
	{
		std::for_each(actions.begin(), actions.end(), [target](std::unique_ptr<Action> &var) {
			var->SetTarget(target);
		});
	}

}
