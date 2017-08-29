#pragma once
#include "Interpolator.hpp"
class AnticipateOvershootInterpolator :
	public Interpolator
{
private:
	float subcat(float t, float s) const {
		return t * t * ((s + 1) * t - s);
	}
	float addcat(float t, float s) const {
		return t * t * ((s + 1) * t + s);
	}
public:
	virtual float Calculate(float timeRatio) const
	{
		if (timeRatio < 0.5f)
			return 0.5f * subcat(timeRatio * 2.0f, factor);
		else
			return 0.5f * ( addcat(timeRatio * 2.0f - 2.0f, factor) + 2.0f);
	}
	
	InterpolatorSingletonWithOneFactor(AnticipateOvershootInterpolator)
};

