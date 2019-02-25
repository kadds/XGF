#include "..\..\Include\Actions.hpp"
#include "../../Include/Action.hpp"
namespace XGF
{
	Actions::Actions() :mIsActionBegan(false), mIsPause(false)
	{
	}


	Actions::~Actions()
	{

	}

	void Actions::Update(float time)
	{
		if (mIsActionBegan & !mIsPause)
		{
			bool rt = false;
			for(auto it = mActions.begin(); it != mActions.end();)
			{
				if (!(*it)->Tick(time))
				{
					rt = true;
					++it;
				}
				else
				{
					it = mActions.erase(it);
				}
			}
			mIsActionBegan = rt;
		}
	}

	void Actions::Pause()
	{
		mIsPause = true;
	}

	void Actions::Resume()
	{
		mIsPause = false;
	}

	bool Actions::IsPause() const
	{
		return mIsPause;
	}

	bool Actions::IsStart() const
	{
		return mIsActionBegan;
	}

	void Actions::AddAction(std::unique_ptr<Action> act)
	{
		mIsActionBegan = true;
		mActions.push_back(std::move(act));
	}
}

