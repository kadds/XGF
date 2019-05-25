#include "./../../../Include/Material/PhysicsMaterial.hpp"

namespace XGF::Shape
{
	PhysicsMaterial::PhysicsMaterial(const Color3& color, const Color3& emissive, float roughness, float metalness, float albedo, float fresnel, Texture* map) :
		LightMaterial(MaterialType::PhysicsMaterial) {
		mInnerData.mColor = color;
		mInnerData.mEmissive = emissive;
		mInnerData.mRoughness = roughness;
		mInnerData.mMetalness = metalness;
		mInnerData.mAlbedo = albedo;
		mInnerData.mFresnel = fresnel;
		mBasicMap = map;
	}
	Color3 PhysicsMaterial::GetColor() const 
	{ 
		return mInnerData.mColor;
	}
	void PhysicsMaterial::SetColor(const Color3& color) 
	{
		mInnerData.mColor = color;
	}
	float PhysicsMaterial::GetMetalness() const
	{ 
		return mInnerData.mMetalness;
	}
	void PhysicsMaterial::SetMetalness(float metalness)
	{
		mInnerData.mMetalness = metalness; 
	}
	void PhysicsMaterial::SetEmissive(const Color3& emissive) 
	{
		mInnerData.mEmissive = emissive; 
	}
	Color3 PhysicsMaterial::GetEmissive() const
	{ 
		return mInnerData.mEmissive;
	}
	float PhysicsMaterial::GetRoughness() const
	{
		return mInnerData.mRoughness;
	}
	void PhysicsMaterial::SetRoughness(float roughness) 
	{
		mInnerData.mRoughness = roughness;
	}
	float PhysicsMaterial::GetAlbedo() const 
	{
		return mInnerData.mAlbedo; 
	}
	void PhysicsMaterial::SetAlbedo(float albedo)
	{
		mInnerData.mAlbedo = albedo;
	}
	float PhysicsMaterial::GetFresnel() const
	{
		return mInnerData.mFresnel;
	}
	void PhysicsMaterial::SetFresnel(float fresnel) 
	{
		mInnerData.mFresnel = fresnel;
	}
	void PhysicsMaterial::SetBasicTexture(Texture* texture)
	{ 
		mBasicMap = texture;
	}
	Texture* PhysicsMaterial::GetBasicTexture() const
	{
		return mBasicMap; 
	}
}