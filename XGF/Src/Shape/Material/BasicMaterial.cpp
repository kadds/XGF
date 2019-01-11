#include "..\..\..\Include\Material\BasicMaterial.hpp"
#include "..\..\..\Include\Geometry\Geometry.hpp"
#include "../../../Include/Context.hpp"
#include "../../../Include/ShaderManager.hpp"

namespace XGF::Shape
{
	BasicMaterial::BasicMaterial(const Color& basicColor, Texture* basicTexture): mBasicColor(basicColor),
	                                                                              mBasicMap(basicTexture), mDFlag(true)
	{
		if (basicTexture)
			SetShaders(Context::Current().QueryShaderManager().GetBasicShaders(true, true, false));
		else
			SetShaders(Context::Current().QueryShaderManager().GetBasicShaders(true, false, false));
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
		if (texture)
			SetShaders(Context::Current().QueryShaderManager().GetBasicShaders(true, true, false));
		else
			SetShaders(Context::Current().QueryShaderManager().GetBasicShaders(true, false, false));
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

	std::vector<void*> BasicMaterial::GetPSBindingConstantBuffers()
	{
		auto r = std::vector<void *>();
		r.push_back(&mBasicColor);
		return r;
	}

	std::vector<Texture*> BasicMaterial::GetPSBindingTextures()
	{
		auto r = std::vector<Texture *>();
		if(mBasicMap)
			r.push_back(mBasicMap);
		return r;
	}
}
