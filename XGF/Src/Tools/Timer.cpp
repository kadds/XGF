#include "../../Include/Timer.hpp"
namespace XGF
{
	Timer::Timer()
	{
		QueryPerformanceFrequency(&mFreq);
		mSecondsPerCount = 1.0 / static_cast<double>(mFreq.QuadPart);
		QueryPerformanceCounter(&mTime);
	}


	Timer::~Timer()
	{
	}

	float Timer::Tick()
	{
		LARGE_INTEGER currTime;
		QueryPerformanceCounter(&currTime);
		float dt = static_cast<float>((currTime.QuadPart - mTime.QuadPart) * mSecondsPerCount);
		mTime = currTime;
		return dt * mFactor;
	}

	void Timer::SetFactor(float factor)
	{
		mFactor = factor;
	}

	float Timer::GetTime(long delta)
	{
		return static_cast<float>(delta * mSecondsPerCount);
	}

	LONGLONG Timer::GetTimeCount()
	{
		LARGE_INTEGER currTime;
		QueryPerformanceCounter(&currTime);
		return currTime.QuadPart;
	}

}
