#pragma once
#include "./../Defines.hpp"
#include "LightMaterial.hpp"

namespace  XGF::Shape
{
	class PhysicsMaterial final : public LightMaterial
	{
	private:
	public:
		PhysicsMaterial(): LightMaterial(MaterialType::PhysicsMaterial) {  }
	private:
		struct
		{
			Color3 mColor;
			float _0;
			Color3 mEmissive;
			float _1;
			float mRoughness;
			float mMetalness;
			float mAlbedo;
			float mFresnel;
		} mInnerData;
	};
}
