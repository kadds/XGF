#pragma once
#include <Windows.h>
#include "Defines.hpp"
namespace XGF
{
	/*
	*�߾��ȼ�ʱ��
	*��Ϸ������ʱ��
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
