#pragma once
#include "Defines.hpp"
#include "Timer.hpp"

namespace XGF
{
	class FrameRateLimiter
	{
	public:
		FrameRateLimiter(int frameRate = 0);

		float Tick();
		void TickStand();
		bool CanExec();

		bool ShouldDiscard();

		int GetStandFrameRate() const;
		void SetFrameRate(int frameRate);

		float GetStandCost() const;
		float GetRestTime();

		float& CurrentRecord();

		float& NextRecord();

		float RecentCost();
	private:
		static const int MaxLastFrameRecord = 10;
		float mStandCost;
		int mFrameRate;
		unsigned int mCurrentFrame;
		float mLastTimeCost[MaxLastFrameRecord];
		float mOverCost;
		Timer mTimer;
		Timer mFrameTimer;
	};
}
