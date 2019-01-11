#include  "./../../../Include/Light/DirectionalLight.hpp"

namespace XGF::Shape
{
	DirectionalLight::DirectionalLight(const Point& direction, const Color& lightColor): Light(Point::Zero, lightColor,
	                                                                                           LightType::Directional),
	                                                                                     mLightDirection(direction)
	{
	}

	int DirectionalLight::CopyConstantBuffer(void* dest)
	{
		Color* destc = (Color *)dest;
		*destc++ = GetLightColor();
		*((Point*)destc) = mLightDirection;
		return sizeof(Color) * 2;
	}

	void DirectionalLight::SetDirection(const Point& direction)
	{
		mLightDirection = direction;
	}

	Point DirectionalLight::GetDirection() const
	{
		return mLightDirection;
	}
}
