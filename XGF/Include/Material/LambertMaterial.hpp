#pragma once
#include "./../Defines.hpp"
#include "LightMaterial.hpp"

namespace XGF::Shape
{
	class LambertMaterial : public LightMaterial
	{
	public:
		LambertMaterial(const Color3 & color, Texture * texture = nullptr, const Color3 &  diffuse = Color3(0.8f, 0.8f, 0.8f), const Color3 & emissive = Color3(0, 0, 0));

		Color3 GetColor() const;

		void SetColor(const Color3& color);

		void SetEmissive(const Color3& emissive);

		Color3 GetEmissive() const;

		Color3 GetDiffuse() const;

		void SetDiffuse(const Color3 & diffuse);
		void SetBasicTexture(Texture * texture);

		Texture* GetBasicTexture() const;


	private:
		struct InnerData
		{
			Color3 mColor;
			float _0;
			Color3 mEmissive;
			float _1;
			Color3 mDiffuse;
			float _2;
		} mInnerData;


		Texture * mBasicMap;
	public:
		const InnerData & GetStructData()
		{
			return mInnerData;
		}
	};
}
