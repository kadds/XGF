#pragma once
#include "Interpolator.hpp"
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

