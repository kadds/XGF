#include "../../Include/Action.hpp"



Action::Action():mID(0)
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
