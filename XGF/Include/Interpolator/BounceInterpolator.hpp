#pragma once
#include "Interpolator.hpp"
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
	float bounce(float t) const{ return t*t * 8.0f; }
	InterpolatorSingleton(BounceInterpolator)
};


