#pragma once
#include "./../Defines.hpp"

namespace XGF 
{
	namespace Shape
	{
		enum class MaterialType
		{
			BasicMaterial = 1u,
			PhongMaterial,
			LambertMaterial,
			PhysicsMaterial,
		};
		class Material
		{
		public:
			Material(MaterialType type);
			virtual ~Material();

			bool IsLinkWithLight() const
			{
				return mLinkWithLight;
			}
			MaterialType GetMaterialType() const
			{
				return mMaterialType;
			}
		protected:
			bool mLinkWithLight;
			MaterialType mMaterialType;
		};
	}
}