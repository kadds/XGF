#include "./../../../Include/Material/LightMaterial.hpp"
namespace XGF::Shape
{
	bool LightMaterial::CanLinkWithLight()
	{
		return true;
	}

	int LightMaterial::GetLightGroup() const
	{
		return mLightGroup;
	}

	void LightMaterial::SetLightGroup(int lightGroup)
	{
		mLightGroup = lightGroup;
	}

	void LightMaterial::AddLight(Light* light)
	{
		mLights[(int)light->GetLightType()].push_back(light);
	}

	void LightMaterial::RemoveLight(Light* light)
	{
		auto& vec = mLights[(int)light->GetLightType()];
		auto it = std::find(vec.begin(), vec.end(), light);
		if (it != vec.end())
		{
			vec.erase(it);
		}
	}

	std::vector<Light*>* LightMaterial::GetLights()
	{
		return mLights;
	}

	void LightMaterial::BeginSetLights()
	{
		for (int i = 0; i < (int)LightType::InvalidValue; i++)
		{
			mLights[i].clear();
		}
	}

	void LightMaterial::EndSetLights()
	{
		
	}

	bool LightMaterial::ShouldWriteBufferLight() const
	{
		for (int i = 1; i < (int)LightType::InvalidValue; i++)
		{
			if (!mLights[i].empty()) return true;
		}
		return false;
	}
}
