#pragma once
#include "./../Defines.hpp"
#include "Material.hpp"
#include "./../Light/Light.hpp"
#include "../ShaderManager.hpp"
#include "../Shadow.hpp"

namespace XGF::Shape
{
	class LightMaterial: public Material, public ReceiveShadowAble
	{
	public:
		LightMaterial(MaterialType type);
		int GetLightGroup() const;

		void SetLightGroup(int lightGroup);

		bool IsUseViewPosition() const
		{
			return mUseViewPosition;
		}

		virtual void OnReceiveShadowPropertyChanged() override;
		ClassProperty(AmbientLightColor, Color)
		ClassProperty(CastShadowLights, std::vector<CastShadowAbleLight *>)
		ClassProperty(NoneCastShadowLights, std::vector<CastShadowAbleLight *>)
	private:
		int mLightGroup = 0;
	protected:

		bool mUseViewPosition;
	};
}
