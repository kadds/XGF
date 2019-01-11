#include "./../../../Include/Light/PointLight.hpp"

namespace XGF::Shape
{
	PointLight::PointLight(const Point& position, const Color& lightColor, float constant, float linear,
	                       float quadratic): Light(position, lightColor, LightType::Point),
	                                         mAttenuation(constant, linear, quadratic)
	{
	}

	PointLight::
	PointLight(const Point& position, const Color& lightColor, const Point& attenuation): Light(position, lightColor,
	                                                                                            LightType::Point),
	                                                                                      mAttenuation(attenuation)
	{
	}

	void PointLight::SetAttenuation(const Point& atte)
	{	
		mAttenuation = atte;
	}

	void PointLight::SetAttenuationConstant(float constant)
	{
		mAttenuation.x = constant;
	}

	void PointLight::SetAttenuationLinear(float linear)
	{
		mAttenuation.y = linear;
	}

	void PointLight::SetAttenuationQuadratic(float quadratic)
	{
		mAttenuation.z = quadratic;
	}

	float PointLight::GetAttenuationQuadratic() const
	{
		return mAttenuation.z;
	}

	float PointLight::GetAttenuationLinear() const
	{
		return mAttenuation.y;
	}

	float PointLight::GetAttenuationConstant() const
	{
		return mAttenuation.x;
	}

	Point PointLight::GetAttenuation() const
	{
		return mAttenuation;
	}

	int PointLight::CopyConstantBuffer(void* dest)
	{
		Color* destc = (Color *)dest;
		*destc++ = GetLightColor();
		*((Point*)destc) = GetTransform().GetTranslate();
		++destc;
		*((Point*)destc) = mAttenuation;
		return sizeof(Color) * 3;
	}
}
