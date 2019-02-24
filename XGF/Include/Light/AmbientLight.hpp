#pragma once
#include "../Defines.hpp"
#include "Light.hpp"

namespace XGF::Shape
{
	class AmbientLight : public Light
	{
	public:
		AmbientLight(const Color& lightColor);

		Color GetAmbientColor() const;

	private:
	};
}
