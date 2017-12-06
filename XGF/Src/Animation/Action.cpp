#include "../../Include/Action.hpp"


namespace XGF
{
	Action::Action() :mID(0)
	{
	}


	Action::~Action()
	{
	}

	void Action::Reset()
	{
		mBegan = false;
		mComplete = false;
	}

}
