#pragma once
#include "Interpolator.hpp"
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

