#pragma once
#include "Interpolator.hpp"
class LinearInterpolator:public Interpolator
{
public:
	virtual float Calculate(float timeRatio) const override
	{
		return  timeRatio;
	}
	InterpolatorSingleton(LinearInterpolator)
};

