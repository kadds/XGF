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

	float Timer::TickWith(float time)
	{
		LARGE_INTEGER currTime;
		QueryPerformanceCounter(&currTime);
		mTime.QuadPart = currTime.QuadPart - static_cast<LONGLONG>(time / mSecondsPerCount);
		return time;
	}

	float Timer::TickWithoutRecord()
	{
		LARGE_INTEGER currTime;
		QueryPerformanceCounter(&currTime);
		float dt = static_cast<float>((currTime.QuadPart - mTime.QuadPart) * mSecondsPerCount);
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

	
	Scheduler::Scheduler()
	{

	}

	void Scheduler::DoSchedule()
	{
		float dt = mTimer.Tick();
		for (auto & schedule : schedules)
		{
			schedule.deltaTimePass += dt;
			if(schedule.deltaTimePass >= schedule.tick)
			{
				auto s = schedule.deltaTimePass;
				schedule.deltaTimePass = 0.f;
				schedule.func(s);
			}
		}
	}

	void Scheduler::AddSchedule(ScheduleInfo info)
	{
		auto it = std::lower_bound(schedules.begin(), schedules.end(), info,
		                           [](const ScheduleInfo& info1, const ScheduleInfo& info2)
		                           {
			                           return info1.tick < info2.tick;
		                           });
		schedules.insert(it, info);
	}

	void Scheduler::RemoveSchedule(const std::string& name)
	{
		auto it = std::find_if(schedules.begin(), schedules.end(), [&name] (const ScheduleInfo & info)
		{
			return info.name == name;
		});
		if(it != schedules.end())
		{
			schedules.erase(it);
		}
	}

	ScheduleInfo Scheduler::GetSchedule(const std::string& name)
	{
		auto it = std::find_if(schedules.begin(), schedules.end(), [&name](const ScheduleInfo & info)
		{
			return info.name == name;
		});
		if (it != schedules.end())
		{
			return *it;
		}
		return ScheduleInfo();
	}

	float Scheduler::GetDeltaTimeFromLastSchedule()
	{
		return mTimer.Tick();
	}
}
