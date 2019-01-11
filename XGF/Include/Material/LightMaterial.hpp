#pragma once
#include "./../Defines.hpp"
#include "Material.hpp"
#include "./../Light/Light.hpp"
#include "../ShaderManager.hpp"

namespace XGF::Shape
{
	class LightMaterial: public Material
	{
	public:
		LightMaterial(){  }

		virtual bool CanLinkWithLight() override;

		int GetLightGroup() const;

		void SetLightGroup(int lightGroup);

		std::vector<Light *>* GetLights();

		void BeginSetLights();
		void AddLight(Light* light);

		void RemoveLight(Light* light);
		virtual  void EndSetLights();

		virtual  void CompileShader() = 0;
		bool IsUseViewPosition() const
		{
			return mUseViewPosition;
		}

		bool ShouldWriteBufferLight() const;
	private:
		int mLightGroup = 0;
	protected:
		std::vector<Light *> mLights[(int)LightType::InvalidValue];
		bool mUseViewPosition;
	};
}
