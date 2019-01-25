#include "..\..\Include\FrameRateLimiter.hpp"

namespace  XGF
{
	FrameRateLimiter::FrameRateLimiter(int frameRate): mCurrentFrame(0.f), mFrameRate(frameRate), mOverCost(0.f)
	{
		if (frameRate != 0)
		{
			mStandCost = 1.f / frameRate;
		}
		else
		{
			mStandCost = 0;
		}
		for (auto& lastTimeCost : mLastTimeCost)
		{
			lastTimeCost = mStandCost;
		}
	}

	float FrameRateLimiter::Tick()
	{
		auto t = mTimer.Tick();
		NextRecord() = t;
		return mFrameTimer.Tick();
	}

	void FrameRateLimiter::TickStand()
	{
		NextRecord() = mTimer.TickWith(mStandCost);
		mFrameTimer.Tick();
	}

	bool FrameRateLimiter::CanExec()
	{
		if (mFrameRate == 0)
			return true;
		auto dt = mTimer.Tick();
		CurrentRecord() += dt;
		float dcost = RecentCost() - mStandCost;
		if(dcost >= 0)
		{
			//CurrentRecord() += mOverCost;
			return true;
		}
		return false;
	}

	bool FrameRateLimiter::ShouldDiscard()
	{
		if (mFrameRate == 0)
			return false;
		CurrentRecord() += mTimer.Tick();
		return RecentCost() >= mStandCost * 2;
	}

	int FrameRateLimiter::GetStandFrameRate() const
	{
		return mFrameRate;
	}

	void FrameRateLimiter::SetFrameRate(int frameRate)
	{
		mFrameRate = frameRate;
		if (frameRate != 0)
		{
			mStandCost = 1.f / frameRate;
		}
		else
		{
			mStandCost = 0;
		}
		for (auto& lastTimeCost : mLastTimeCost)
		{
			if(lastTimeCost == 0.f)
				lastTimeCost = mStandCost;
		}
	}


	float FrameRateLimiter::GetStandCost() const
	{
		return mStandCost;
	}

	float FrameRateLimiter::GetRestTime()
	{
		if (mFrameRate == 0)
			return 0;
		CurrentRecord() += mTimer.Tick();
		return mStandCost - RecentCost();
	}

	float& FrameRateLimiter::CurrentRecord()
	{
		return mLastTimeCost[mCurrentFrame % MaxLastFrameRecord];
	}

	float& FrameRateLimiter::NextRecord()
	{
		return mLastTimeCost[++mCurrentFrame % MaxLastFrameRecord];
	}

	float FrameRateLimiter::RecentCost()
	{
		float sum = 0;
		for (auto lastTimeCost : mLastTimeCost)
		{
			sum += lastTimeCost;
		}
		return sum / MaxLastFrameRecord;
	}
}
