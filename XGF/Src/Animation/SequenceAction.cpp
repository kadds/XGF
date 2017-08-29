#include "..\..\Include\SequenceAction.hpp"
#include "..\..\Include\Animation.hpp"


SequenceAction::SequenceAction()
{

}


SequenceAction::~SequenceAction()
{
}

bool SequenceAction::Tick(float time)
{
	bool isEnd = false;
	size_t p = 0;
	
	if (actions[nowAction]->Tick(time - mTime))
	{
		mTime = time;//+ mDelaytime;
		if (actions.size() == nowAction + 1)
		{
			return true;
		}
		else
		{
			nowAction ++;
		}
	}
	return false;
}

void SequenceAction::Reset()
{
	CompositeAction::Reset();
	mTime = 0.f;
	nowAction = 0;
}
