#pragma once
#include "../Defines.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>
#include <mutex>
#include <memory>
namespace XGF
{
	//view this: http://inloop.github.io/interpolator/
	class Interpolator
	{
	public:

		virtual float Calculate(float timeRatio) const = 0;

	protected:
		Interpolator() {}
		~Interpolator() {}
	};


#define InterpolatorSingleton(interpolator) public:\
interpolator(){}\
~interpolator(){}\
public:\
static std::shared_ptr<interpolator> GetInterpolator()\
{\
	return mInterpolator;\
}\
private:\
static std::shared_ptr<interpolator> mInterpolator;

#define InterpolatorSingletonWithOneFactor(interpolator) private:\
static std::mutex mutex;\
float factor;\
public:\
interpolator(float f):factor(f){}\
~interpolator(){}\
public:\
static std::shared_ptr<interpolator> GetInterpolator(float ffactor)\
{\
	std::lock_guard<std::mutex>lk(mutex);\
	return std::make_shared<interpolator>(ffactor);\
}


#define InterpolatorSingletonDefineWithOneFactor(interpolator) std::mutex interpolator::mutex;
#define InterpolatorSingletonDefine(interpolator) std::shared_ptr<interpolator> interpolator::mInterpolator = std::make_shared<interpolator>();

	class AccelerateDecelerateInterpolator :public Interpolator
	{
	public:
		virtual float Calculate(float timeRatio) const override
		{
			return  (float)(cos((timeRatio + 1) * M_PI) / 2.0f) + 0.5f;
		}
		InterpolatorSingleton(AccelerateDecelerateInterpolator)
	};
	//factor set 1.0 is great;
	class AccelerateInterpolator :
		public Interpolator
	{
	public:
		virtual float Calculate(float timeRatio) const
		{
			if (factor == 1.0)
				return timeRatio * timeRatio;
			else
				return pow(timeRatio, 2 * factor);
		}
		InterpolatorSingletonWithOneFactor(AccelerateInterpolator)
	};

	//factor (tension) set 2.0 is great;
	class AnticipateInterpolator :
		public Interpolator
	{
	public:
		virtual float Calculate(float timeRatio) const
		{
			return timeRatio * timeRatio * ((factor + 1) * timeRatio - factor);
		}
		InterpolatorSingletonWithOneFactor(AnticipateInterpolator)
	};

	class AnticipateOvershootInterpolator :
		public Interpolator
	{
	private:
		float subcat(float t, float s) const {
			return t * t * ((s + 1) * t - s);
		}
		float addcat(float t, float s) const {
			return t * t * ((s + 1) * t + s);
		}
	public:
		virtual float Calculate(float timeRatio) const
		{
			if (timeRatio < 0.5f)
				return 0.5f * subcat(timeRatio * 2.0f, factor);
			else
				return 0.5f * (addcat(timeRatio * 2.0f - 2.0f, factor) + 2.0f);
		}

		InterpolatorSingletonWithOneFactor(AnticipateOvershootInterpolator)
	};
	class BounceInterpolator :
		public Interpolator
	{
	public:
		virtual float Calculate(float timeRatio) const
		{
			if (timeRatio < 0.3535f)
				return bounce(timeRatio);
			else if (timeRatio < 0.7408f)
				return bounce(timeRatio - 0.54719f) + 0.7f;
			else if (timeRatio < 0.9644f)
				return bounce(timeRatio - 0.8526f) + 0.9f;
			else
				return bounce(timeRatio - 1.0435f) + 0.95f;
		}
		float bounce(float t) const { return t*t * 8.0f; }
		InterpolatorSingleton(BounceInterpolator)
	};
	class CycleInterpolator :
		public Interpolator
	{
	public:
		virtual float Calculate(float timeRatio) const
		{
			return sin(factor * static_cast<float>(M_PI) * timeRatio);
		}
		InterpolatorSingletonWithOneFactor(CycleInterpolator)
	};
	class DecelerateInterpolator :
		public Interpolator
	{
	public:
		virtual float Calculate(float timeRatio) const
		{
			if (factor == 1.0f)
				return (1.0f - (1.0f - timeRatio) * (1.0f - timeRatio));
			else
				return (1.0f - static_cast<float>(pow((1.0f - timeRatio), 2 * factor)));
		}
		InterpolatorSingletonWithOneFactor(DecelerateInterpolator)
	};
	class LinearInterpolator :public Interpolator
	{
	public:
		virtual float Calculate(float timeRatio) const override
		{
			return  timeRatio;
		}
		InterpolatorSingleton(LinearInterpolator)
	};

	class OvershootInterpolator :
		public Interpolator
	{
	public:
		virtual float Calculate(float timeRatio) const
		{
			timeRatio -= 1.0f;
			return timeRatio * timeRatio * ((factor + 1) * timeRatio + factor) + 1.0f;
		}
		InterpolatorSingletonWithOneFactor(OvershootInterpolator)
	};

	//factor set 0.4 is great;
	class SpringInterpolator :
		public Interpolator
	{
	public:
		virtual float Calculate(float timeRatio) const
		{
			return static_cast<float>(pow(2, -10.f * timeRatio) * sin((timeRatio - factor / 4.0f) * (2.0f * static_cast<float>(M_PI)) / factor) + 1.0f);
		}
		InterpolatorSingletonWithOneFactor(SpringInterpolator)
	};



}
