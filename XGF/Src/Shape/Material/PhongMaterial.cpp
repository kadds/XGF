#include "..\..\..\Include\Material\PhongMaterial.hpp"
#include "../../../Include/Context.hpp"

namespace XGF::Shape
{
	PhongMaterial::PhongMaterial(const Color3 &  color, Texture * texture, const Color3 &  diffuse, const Color3 &  specular, float shininess, const Color3 & emissive) : LightMaterial(MaterialType::PhongMaterial)
	{
		mInnerData.mColor = color;
		mInnerData.mEmissive = emissive;
		mInnerData.mSpecular = specular;
		mInnerData.mDiffuse = diffuse;
		mInnerData.mShininess = shininess;
		mBasicMap = texture;
	}
	Texture* PhongMaterial::GetBasicTexture() const
	{
		return mBasicMap;
	}

	void PhongMaterial::SetBasicTexture(Texture * texture)
	{
		mBasicMap = texture;
	}
	Color3 PhongMaterial::GetColor() const
	{
		return mInnerData.mColor;
	}

	void PhongMaterial::SetColor(const Color3& color)
	{
		mInnerData.mColor = color;
	}

	Color3 PhongMaterial::GetSpecular() const
	{
		return mInnerData.mSpecular;
	}

	void PhongMaterial::SetEmissive(const Color3& emissive)
	{
		mInnerData.mEmissive = emissive;
	}

	Color3 PhongMaterial::GetEmissive() const
	{
		return mInnerData.mEmissive;
	}

	void PhongMaterial::SetSpecular(const Color3& specular)
	{
		mInnerData.mSpecular = specular;
	}

	float PhongMaterial::GetShininess() const
	{
		return mInnerData.mShininess;
	}

	void PhongMaterial::SetShininess(float shininess)
	{
		mInnerData.mShininess = shininess;
	}

	Color3 PhongMaterial::GetDiffuse() const
	{
		return mInnerData.mDiffuse;
	}

	void PhongMaterial::SetDiffuse(const Color3 & emissive)
	{
		mInnerData.mDiffuse = emissive;
	}
}
