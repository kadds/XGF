#include "./../../../Include/Light/AmbientLight.hpp"

namespace XGF::Shape
{
	AmbientLight::AmbientLight(const Color& lightColor): Light(Point::Zero, lightColor, LightType::Ambient)
	{
	}
	Color AmbientLight::GetAmbientColor() const
	{
		return GetLightColor();
	}
}
