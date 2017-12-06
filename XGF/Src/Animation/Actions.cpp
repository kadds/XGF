#include "..\..\Include\Actions.hpp"
#include "../../Include/Action.hpp"
namespace XGF
{
	Actions::Actions() :onActionCompletedListener(nullptr), mIsActionBegan(false)
	{
	}


	Actions::~Actions()
	{
		//delete mAction;
	}

	void Actions::Updata(float time)
	{
		if (mIsActionBegan && mAction != nullptr)
		{
			mPassedTime += time;
			if (mAction->Tick(mPassedTime))
			{
				mIsActionBegan = false;
				if (onActionCompletedListener != nullptr)
				{
					onActionCompletedListener();
				}
			}
		}
	}

	void Actions::Start()
	{
		if (mAction != nullptr)
		{
			mAction->Reset();
			mIsActionBegan = true;
			mPassedTime = 0.f;
		}
	}

	void Actions::Stop()
	{
	}

	bool Actions::IsStart()
	{
		return mIsActionBegan;
	}

	void Actions::SetAction(std::unique_ptr<Action> action, ActionInterface * ainterface)
	{
		mAction = std::move(action);
		if (mAction != nullptr)
			mAction->SetTarget(ainterface);
	}

}

