#include "..\..\..\Include\Material\PhongMaterial.hpp"
#include "../../../Include/Context.hpp"

namespace XGF::Shape
{
	PhongMaterial::PhongMaterial(const Color3 &  color, Texture * texture, const Color3 &  diffuse, const Color3 &  specular, float shininess, const Color3 & emissive)//: mColor(color),
	                                                                                             //  mSpecular(specular),
	                                                                                             //  mShininess(shininess),
	                                                                                             //  mEmissive(emissive)
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

	std::vector<Texture*> PhongMaterial::GetPSBindingTextures()
	{
		std::vector<Texture *> textures;
		if(mBasicMap)
			textures.push_back(mBasicMap);
		return textures;
	}
	
	std::vector<void*> PhongMaterial::GetPSBindingConstantBuffers()
	{
		std::vector<void *> v;
		v.push_back(&mInnerData);
		return v;
	}

	void PhongMaterial::SetBasicTexture(Texture * texture)
	{
		mBasicMap = texture;
		CompileShader();
	}
	void PhongMaterial::EndSetLights()
	{
		auto lights = GetLights();
		for (int i = 1; i < (int)LightType::InvalidValue; i++)
		{
			if (!lights[i].empty())
			{
				mUseViewPosition = true;
				return;
			}
		}
		mUseViewPosition = false;
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
	void PhongMaterial::CompileShader()
	{
		int numOfDir = static_cast<int>(mLights[static_cast<int>(LightType::Directional)].size());
		int numOfPoint = static_cast<int>(mLights[static_cast<int>(LightType::Point)].size());
		int numOfSpot = static_cast<int>(mLights[static_cast<int>(LightType::Spot)].size());
		Shaders s = Context::Current().QueryShaderManager().GetPhongShaders(
			numOfDir, numOfPoint, numOfSpot, mBasicMap != nullptr);
		SetShaders(s);
	}
}
