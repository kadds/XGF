#pragma once
#include "Material.hpp"
#include "../ConstantData.hpp"

namespace XGF
{
	namespace Shape
	{
		class Geometry;
		class BasicMaterial : public Material
		{
		public:
			BasicMaterial(const SM::Color & color);
			virtual ~BasicMaterial();
			virtual BindingBridge CreateBinders(const Geometry & geometry);
		private:
			SM::Color baseColor;
		};
	}
}