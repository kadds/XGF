#pragma once
#include "Material.hpp"
#include "../ConstantData.hpp"
namespace XGF
{
	namespace Shape
	{
		class BasicMaterial : public Material
		{
		public:
			BasicMaterial(const SM::Color & color);
		private:
			SM::Color baseColor;
		};
	}
}