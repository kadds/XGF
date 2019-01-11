#include "..\..\..\Include\Material\LambertMaterial.hpp"
#include "../../../Include/Context.hpp"

namespace XGF::Shape
{
	LambertMaterial::LambertMaterial(const Color3 &  color, Texture * texture, const Color3 &  diffuse, const Color3 & emissive)//: mColor(color),
																								 //  mSpecular(specular),
																								 //  mShininess(shininess),
																								 //  mEmissive(emissive)
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

	std::vector<Texture*> LambertMaterial::GetPSBindingTextures()
	{
		std::vector<Texture *> textures;
		if (mBasicMap)
			textures.push_back(mBasicMap);
		return textures;
	}

	std::vector<void*> LambertMaterial::GetPSBindingConstantBuffers()
	{
		std::vector<void *> v;
		v.push_back(&mInnerData);
		return v;
	}

	void LambertMaterial::EndSetLights()
	{
		mUseViewPosition = false;
	}

	void LambertMaterial::SetBasicTexture(Texture * texture)
	{
		mBasicMap = texture;
		CompileShader();
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
	void LambertMaterial::CompileShader()
	{
		int numOfDir = static_cast<int>(mLights[static_cast<int>(LightType::Directional)].size());
		int numOfPoint = static_cast<int>(mLights[static_cast<int>(LightType::Point)].size());
		int numOfSpot = static_cast<int>(mLights[static_cast<int>(LightType::Spot)].size());
		Shaders s = Context::Current().QueryShaderManager().GetLambertShaders(
			numOfDir, numOfPoint, numOfSpot, mBasicMap != nullptr);
		SetShaders(s);
	}
}
