#pragma once
#include "Interpolator.hpp"

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

