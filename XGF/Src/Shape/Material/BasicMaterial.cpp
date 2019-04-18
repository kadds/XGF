#include "..\..\..\Include\Material\BasicMaterial.hpp"

namespace XGF::Shape
{
	BasicMaterial::BasicMaterial(const Color& basicColor, Texture* basicTexture): Material(MaterialType::BasicMaterial), mBasicColor(basicColor),
	                                                                              mBasicMap(basicTexture), mDFlag(true)
	{
	}

	BasicMaterial::~BasicMaterial()
	{
	}

	void BasicMaterial::SetBasicColor(Color color)
	{
		mBasicColor = color;
		mDFlag = true;
	}

	void BasicMaterial::SetBasicTexture(Texture * texture)
	{
		mBasicMap = texture;
		mDFlag = true;
	}

	Color BasicMaterial::GetBasicColor() const
	{
		return mBasicColor;
	}

	Texture* BasicMaterial::GetBasicTexture() const
	{
		return mBasicMap;
	}

	bool BasicMaterial::DFlag()
	{
		bool m = mDFlag;
		mDFlag = false;
		return m;
	}

}
