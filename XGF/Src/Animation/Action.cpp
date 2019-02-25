#include "../../Include/Action.hpp"


namespace XGF
{
	Action::Action(float duration): mDuration(duration), mPassedTime(0.f)
	{
	}

	bool Action::Tick(float deltaTime)
	{
		mPassedTime += deltaTime;
		float k = mPassedTime / mDuration;
		if (k > 1.f) k = 1.f;
		return OnTick(k, deltaTime);
	}

	void Action::Reset()
	{
		mPassedTime = 0.f;
		OnReset();
	}

	DataBaseAction::DataBaseAction(float duration, std::shared_ptr<Interpolator> interpolator, NotificationCallback callback): Action(duration),
	                                                                                            mInterpolator(interpolator),
	                                                                                            mCallback(callback), mLastK(0.f)
			
	{
	}

	void DataBaseAction::SetNotificationCallback(NotificationCallback callback)
	{
		mCallback = callback;
	}

	bool DataBaseAction::OnTick(float k, float deltaTime)
	{
		k = mInterpolator->Calculate(k);
		Calc(k - mLastK);
		mLastK = k;
		if (mCallback)
			mCallback();
		return k >= 1;
	}

	void DataBaseAction::OnReset()
	{
		mLastK = 0;
	}


	RepeatAction::RepeatAction(std::unique_ptr<Action> action, int count): mRawCount(count), 
	mRestCount(count), Action(0.f), mAction(std::move(action)), mInfinite(count < 0)
	{
	}

	bool RepeatAction::OnTick(float k, float deltaTime)
	{
		if (mAction->Tick(deltaTime))
		{
			if (!mInfinite) 
			{
				mRestCount--;
				if (mRestCount == 0)
					return true;
			}
			mAction->Reset();
		}
		return false;
	}

	void RepeatAction::OnReset()
	{
		mRestCount = mRawCount;
		mAction->Reset();
	}

	CompositeAction::CompositeAction(std::vector<std::unique_ptr<Action>>&& actions): mActions(std::move(actions)),
	                                                                                  Action(0.0f)
	{
	}

	ParallelAction::ParallelAction(std::vector<std::unique_ptr<Action>>&& actions): CompositeAction(std::move(actions))
	{
	}

	bool ParallelAction::OnTick(float k, float deltaTime)
	{
		bool rt = true;
		for (auto& it : mActions)
		{
			if (!it->Tick(deltaTime))
			{
				rt = false;
			}
		}
		return rt;
	}

	SequenceAction::SequenceAction(std::vector<std::unique_ptr<Action>>&& actions): CompositeAction(std::move(actions)),
	                                                                                mIndex(0)
	{
	}

	bool SequenceAction::OnTick(float k, float deltaTime)
	{
		if (mIndex < mActions.size())
		{
			if (mActions[mIndex]->Tick(deltaTime))
			{
				mIndex++;
			}
		}
		else
		{
			return true;
		}
		return false;
	}

	void SequenceAction::OnReset()
	{
		CompositeAction::OnReset();
		mIndex = 0;
	}

	DelayAction::DelayAction(float duration): Action(duration)
	{
	}

	bool DelayAction::OnTick(float k, float deltaTime)
	{
		if (k >= 1) return true;
		return false;
	}

	void DelayAction::OnReset()
	{
	}
}
