#include "..\..\Include\RepeatAction.hpp"



RepeatAction::RepeatAction()
{
}


RepeatAction::~RepeatAction()
{
}

bool RepeatAction::Tick(float time)
{
	bool isEnd = true;
	std::for_each(actions.begin(), actions.end(), [&](std::unique_ptr<Action> &var) {
		if (!var->Tick(time - mTime))
			isEnd = false;
	});
	if (isEnd)
	{
		if (mNum - mPassNum == 1)
			return true;
		mTime = time;
		if (mNum - mPassNum > 1 )
		{
			CompositeAction::Reset();
			mPassNum ++;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void RepeatAction::Reset()
{
	CompositeAction::Reset();
	mTime = 0.f;
	mPassNum = 0;
}
