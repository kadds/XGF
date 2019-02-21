#pragma once
#include "./../Defines.hpp"
#include "Material.hpp"
namespace XGF::Shape
{
	/**
	 * ²ÄÖÊÑÕÉ«ÎÆÀí
	 */
	class BasicMaterial : public Material
	{
	public:
		BasicMaterial(const Color& basicColor, Texture* basicTexture = nullptr);
		virtual ~BasicMaterial();

		void SetBasicColor(Color color);

		void SetBasicTexture(Texture * texture);

		Color GetBasicColor() const;

		Texture* GetBasicTexture() const;

		bool DFlag();

	private:
		bool mDFlag;
		Color mBasicColor;
		Texture * mBasicMap;
	};

}