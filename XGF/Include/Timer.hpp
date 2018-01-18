#pragma once
#include <Windows.h>
#include "Defines.hpp"
namespace XGF
{
	/*
	*高精度计时器
	*游戏基本计时器
	*/
	class Timer
	{
	public:
		Timer();
		~Timer();
		float Tick();
		void SetFactor(float factor);
		long GetTimeCount();
		float GetTime(long delta);
	private:
		LARGE_INTEGER mFreq;
		LARGE_INTEGER mTime;
		double mSecondsPerCount;
		float mFactor = 1.0f;
	};
}
