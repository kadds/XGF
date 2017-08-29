#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>
#include <mutex>
#include <memory>
//view this: http://inloop.github.io/interpolator/
class Interpolator
{
public:
	
	virtual float Calculate(float timeRatio) const = 0;

protected:
	Interpolator() {}
	~Interpolator(){}
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

