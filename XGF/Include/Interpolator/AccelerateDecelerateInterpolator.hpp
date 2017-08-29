#pragma once
#include "Interpolator.hpp"
class AccelerateDecelerateInterpolator :public Interpolator
{
public:
	virtual float Calculate(float timeRatio) const override
	{
		return  (float)(cos((timeRatio + 1) * M_PI) / 2.0f) + 0.5f;
	}
	InterpolatorSingleton(AccelerateDecelerateInterpolator)
};
