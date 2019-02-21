#include "./../../../Include/Material/LightMaterial.hpp"
namespace XGF::Shape
{
	LightMaterial::LightMaterial(MaterialType type): Material(type)
	{
		mLinkWithLight = true;
	}

	int LightMaterial::GetLightGroup() const
	{
		return mLightGroup;
	}

	void LightMaterial::SetLightGroup(int lightGroup)
	{
		mLightGroup = lightGroup;
	}

	void LightMaterial::OnReceiveShadowPropertyChanged()
	{
	}
}
