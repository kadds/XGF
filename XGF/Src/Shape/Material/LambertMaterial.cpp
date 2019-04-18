#include "..\..\..\Include\Material\LambertMaterial.hpp"

namespace XGF::Shape
{
	LambertMaterial::LambertMaterial(const Color3 &  color, Texture * texture, const Color3 &  diffuse, const Color3 & emissive): LightMaterial(MaterialType::LambertMaterial)

	{
		mInnerData.mColor = color;
		mInnerData.mDiffuse = diffuse;
		mBasicMap = texture;
		mUseViewPosition = false;
	}
	Texture* LambertMaterial::GetBasicTexture() const
	{
		return mBasicMap;
	}

	void LambertMaterial::SetBasicTexture(Texture * texture)
	{
		mBasicMap = texture;
	}
	Color3 LambertMaterial::GetColor() const
	{
		return mInnerData.mColor;
	}

	void LambertMaterial::SetColor(const Color3& color)
	{
		mInnerData.mColor = color;
	}
	void LambertMaterial::SetEmissive(const Color3& emissive)
	{
		mInnerData.mEmissive = emissive;
	}

	Color3 LambertMaterial::GetEmissive() const
	{
		return mInnerData.mEmissive;
	}
	Color3 LambertMaterial::GetDiffuse() const
	{
		return mInnerData.mDiffuse;
	}

	void LambertMaterial::SetDiffuse(const Color3 & diffuse)
	{
		mInnerData.mDiffuse = diffuse;
	}
}
