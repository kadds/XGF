#pragma once
#include <Windows.h>
#include "Defines.hpp"
#include <algorithm>

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
		float TickWith(float time);
		float TickWithoutRecord();
		void SetFactor(float factor);
		LONGLONG GetTimeCount();
		float GetTime(long delta);
	private:
		LARGE_INTEGER mFreq;
		LARGE_INTEGER mTime;
		double mSecondsPerCount;
		float mFactor = 1.0f;
	};
	
	typedef std::function<void(float)>  ScheduleFunc;

	struct ScheduleInfo
	{
		float tick;
		float deltaTimePass;
		ScheduleFunc func;
		std::string name;
		ScheduleInfo(): tick(0.f), deltaTimePass(0.f) {  }
		ScheduleInfo(float msTick, ScheduleFunc func, const std::string & name = ""): tick(msTick), func(func), name(name) , deltaTimePass(0.f){  }
	};
	class Scheduler
	{
	private:
		Timer mTimer;
		std::vector<ScheduleInfo> schedules;

	public:
		Scheduler();

		void DoSchedule();

		void AddSchedule(ScheduleInfo info);
		void RemoveSchedule(const std::string & name);
		ScheduleInfo GetSchedule(const std::string & name);
		float GetDeltaTimeFromLastSchedule();
	};
}
