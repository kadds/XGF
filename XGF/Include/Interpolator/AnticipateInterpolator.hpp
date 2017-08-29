#pragma once
#include "Interpolator.hpp"

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

