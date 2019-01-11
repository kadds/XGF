#pragma once
#include "Material.hpp"
#include "../Geometry/Geometry.hpp"
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

		virtual std::vector<void*> GetPSBindingConstantBuffers() override;
		virtual std::vector<Texture *> GetPSBindingTextures() override;
	private:
		bool mDFlag;
		Color mBasicColor;
		Texture * mBasicMap;
	};

}