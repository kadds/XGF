#pragma once
#include "Interpolator.hpp"
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

