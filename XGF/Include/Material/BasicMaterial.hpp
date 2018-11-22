#pragma once
#include "Material.hpp"
#include "../ConstantData.hpp"
#include "../Geometry/Geometry.hpp"
namespace XGF::Shape
{
	class Geometry;
	class BasicMaterialMapping
	{
	public:
		int operator() (int textureType, int textureIndex)
		{
			switch (textureType)
			{
			case 0: return textureIndex;
			case 1: return textureIndex;
			default:
				XGF_Warn(Render, "can't find textureType ", textureType);
				return 0;
			}
		}

	};
	/**
	 * ²ÄÖÊÑÕÉ«ÎÆÀí
	 */
	class BasicMaterial : public Material
	{
	public:
		BasicMaterial(const SM::Color & color, Texture * basicTexture = nullptr) : basicColor(color), basicMap(basicTexture), dFlag(true)
		{
			mShaders = ConstantData::GetInstance().GetBasicShaders();
		}
		virtual ~BasicMaterial() {};

		void SetBasicColor(SM::Color color)
		{
			basicColor = color;
			dFlag = true;
		}
		void SetBasicTexture(Texture & texture)
		{
			basicMap = &texture;
			dFlag = true;
		}
		SM::Color GetBasicColor() const
		{
			return basicColor;
		}
		Texture * GetBasicTexture() const
		{
			return basicMap;
		}
		bool DFlag()
		{
			bool m = dFlag;
			dFlag = false;
			return m;
		}
		virtual std::vector<Texture *> GetTextureBindingList() const override
		{
			std::vector<Texture *> textures;
			textures.push_back(basicMap);
			return textures;
		};
		virtual std::vector<void *> GetConstantBuffers() override
		{
			std::vector<void *> v;
			v.push_back(&basicColor);
			return v;
		}
	private:
		bool dFlag;
		SM::Color basicColor;
		Texture * basicMap;
	};

}