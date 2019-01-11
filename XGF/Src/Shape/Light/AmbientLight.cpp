#include "./../../../Include/Light/AmbientLight.hpp"

namespace XGF::Shape
{
	AmbientLight::AmbientLight(const Color& lightColor): Light(Point::Zero, lightColor, LightType::Ambient)
	{
	}

	int AmbientLight::CopyConstantBuffer(void* dest)
	{
		return 0;
	}

	Color AmbientLight::GetAmbientColor() const
	{
		return GetLightColor();
	}
}
