#pragma once
#include "Interpolator.hpp"
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

