#include "./../../../Include/Light/SpotLight.hpp"

namespace  XGF::Shape
{
	SpotLight::SpotLight(const Point& position, const Point& direction, const Color& lightColor, float cutoff,
	                     float cutoffOuter, float constant, float linear, float quadratic):
		Light(position, lightColor, LightType::Spot), mDirection(direction), mCutoff(cutoff), mCutoffOuter(cutoffOuter),
		mAttenuation(constant, linear, quadratic)
	{
	}

	SpotLight::SpotLight(const Point& position, const Point& direction, const Color& lightColor, const Point& atten,
	                     float cutoff, float cutoffOuter): Light(position, lightColor, LightType::Spot),
	                                                       mDirection(direction), mCutoff(cutoff),
	                                                       mCutoffOuter(cutoffOuter), mAttenuation(atten)
	{
	}

	void SpotLight::SetAttenuation(const Point& atte)
	{
		mAttenuation = atte;
	}

	void SpotLight::SetAttenuationConstant(float constant)
	{
		mAttenuation.x = constant;
	}

	void SpotLight::SetAttenuationLinear(float linear)
	{
		mAttenuation.y = linear;
	}

	void SpotLight::SetAttenuationQuadratic(float quadratic)
	{
		mAttenuation.z = quadratic;
	}

	float SpotLight::GetAttenuationQuadratic() const
	{
		return mAttenuation.z;
	}

	float SpotLight::GetAttenuationLinear() const
	{
		return mAttenuation.y;
	}

	float SpotLight::GetAttenuationConstant() const
	{
		return mAttenuation.x;
	}

	Point SpotLight::GetAttenuation() const
	{
		return mAttenuation;
	}

	int SpotLight::CopyConstantBuffer(void* dest)
	{
		Color* destc = (Color *)dest;
		*destc++ = GetLightColor();

		*((Point*)destc) = GetTransform().GetTranslate();;
		destc++;
		*((Point*)destc) = mDirection;
		((Point4*)destc)->w = mCutoffOuter;
		destc++;
		*destc = Point4(mCutoff, mAttenuation.x, mAttenuation.y, mAttenuation.z);
		return sizeof(Color) * 4;
	}
}
