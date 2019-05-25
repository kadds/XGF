#pragma once
#include "./../Defines.hpp"
#include "LightMaterial.hpp"

namespace  XGF::Shape
{
	class PhysicsMaterial final : public LightMaterial
	{
	public:
		PhysicsMaterial(const Color3& color, const Color3& emissive, float roughness, float metalness, float albedo, float fresnel, Texture* map = nullptr);
		Color3 GetColor() const;;

		void SetColor(const Color3& color);;

		float GetMetalness() const;;

		void SetMetalness(float metalness);;

		void SetEmissive(const Color3& emissive);;

		Color3 GetEmissive() const;;


		float GetRoughness() const;;

		void SetRoughness(float roughness);;
		float GetAlbedo() const;;

		void SetAlbedo(float albedo);;
		float GetFresnel() const;;

		void SetFresnel(float fresnel);;
		void SetBasicTexture(Texture* texture);;

		Texture* GetBasicTexture() const;;

	private:
		struct InnerData
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

		Texture* mBasicMap;
	public:
		const InnerData& GetStructData()
		{
			return mInnerData;
		}
	};
}
