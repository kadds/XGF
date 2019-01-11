#pragma once
#include "Light.hpp"

namespace XGF::Shape
{
	class AmbientLight : public Light
	{
	public:
		AmbientLight(const Color& lightColor);

		Color GetAmbientColor() const;

		int CopyConstantBuffer(void* dest) override;
	private:
	};
}
