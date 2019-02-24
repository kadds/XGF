#include "..\..\Include\Actions.hpp"
#include "../../Include/Action.hpp"
namespace XGF
{
	Actions::Actions() :mIsActionBegan(false)
	{
	}


	Actions::~Actions()
	{
		//delete mAction;
	}

	void Actions::Update(float time)
	{
		if (mIsActionBegan )
		{
			for (auto it = mAction.begin(); it != mAction.end();)
			{
				if (it->second->Tick(it->first, time))
				{
					it = mAction.erase(it);
				}
				else ++it;
			}
			mIsActionBegan = !mAction.empty();
		}
		
	}

	bool Actions::IsStart()
	{
		return mIsActionBegan;
	}

	void Actions::AddAction(Point & From, std::unique_ptr<Action> act)
	{
		act->SetFrom(From);
		mAction.push_back(std::make_pair(&From, std::move(act)));
		mIsActionBegan = true;
	}

}

