#include "../../Include/SystemShaders.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/ShaderManager.hpp"

namespace XGF
{
	Shaders SystemShaders::GetBasicShaders(int type)
	{
		auto & sm = Context::Current().QueryShaderManager();
		ShaderConfiguration configuration;
		if (type & BasicShader_Normal)
			configuration.defines["USE_NORMAL"] = "0";
		if (type & BasicShader_Texture)
			configuration.defines["USE_TEXTURE"] = "0";
		if (type & BasicShader_VertexColor)
			configuration.defines["USE_COLOR"] = "0";
		configuration.version = "4_0";
		auto vs = sm.LoadShader<VertexShader>("BasicMaterial.hlsl", configuration);
		auto ps = sm.LoadShader<PixelShader>("BasicMaterial.hlsl", configuration);
		return Shaders(vs, ps);
	}
	Shaders SystemShaders::GetFontShaders(bool hasVertexColor)
	{
		auto & sm = Context::Current().QueryShaderManager();
		ShaderConfiguration configuration;
		if (hasVertexColor)
			configuration.defines["USE_COLOR"] = "0";
		configuration.version = "4_0";
		auto vs = sm.LoadShader<VertexShader>("FontMaterial.hlsl", configuration);
		auto ps = sm.LoadShader<PixelShader>("FontMaterial.hlsl", configuration);
		return Shaders(vs, ps);
	}
	Shaders GetForwardPhongShaders(bool base, int shaderType, SystemShaders::ForwardShaderLightType lightType,
		SystemShaders::ForwardShaderShadowType shadowType)
	{
		auto & sm = Context::Current().QueryShaderManager();
		ShaderConfiguration configuration;
		if (shaderType & SystemShaders::PhongForwardShader_Texture)
			configuration.defines["TEXTURE"] = "0";
		if (!(shaderType & SystemShaders::PhongForwardShader_BlinnPhong))
			configuration.defines["LIGHT_PHONG"] = "0";
		if (shadowType != SystemShaders::ForwardShaderShadowType::None)
		{
			if (shadowType == SystemShaders::ForwardShaderShadowType::DEF)
				configuration.defines["HAS_DEF_SAMPLER"] = "0";
			else
				configuration.defines["HAS_PCF_SAMPLER"] = "0";
			configuration.defines["SHADOWMAP_TEXTURE"] = "0";

		}
		switch (lightType)
		{
		case SystemShaders::ForwardShaderLightType::Point:
			configuration.defines["IS_POINT_LIGHT"] = "TRUE";
			break;
		case SystemShaders::ForwardShaderLightType::Spot:
			configuration.defines["IS_SPOT_LIGHT"] = "TRUE";
			break;
		case SystemShaders::ForwardShaderLightType::Directional:
			configuration.defines["IS_DIRECTIONAL_LIGHT"] = "TRUE";
			break;
		default: // void type
			;
		}
		if(base)
			configuration.defines["IS_BASE"] = "TRUE";
		else
			configuration.defines["IS_ADD"] = "TRUE";
		configuration.version = "4_0";
		auto vs = sm.LoadShader<VertexShader>("PhongMaterialF.hlsl", configuration);
		auto ps = sm.LoadShader<PixelShader>("PhongMaterialF.hlsl", configuration);
		return Shaders(vs, ps);
	}
	Shaders SystemShaders::GetForwardBasePhongShaders(int shaderType, ForwardShaderLightType lightType,
		ForwardShaderShadowType shadowType)
	{
		return GetForwardPhongShaders(true, shaderType, lightType, shadowType);
	}

	Shaders SystemShaders::GetForwardAddPhongShaders(int shaderType, ForwardShaderLightType lightType,
		ForwardShaderShadowType shadowType)
	{
		return GetForwardPhongShaders(false, shaderType, lightType, shadowType);
	}
	
	Shaders GetForwardPhysicsShaders(bool base, int shaderType, SystemShaders::ForwardShaderLightType lightType, SystemShaders::ForwardShaderShadowType shadowType)
	{
		auto& sm = Context::Current().QueryShaderManager();
		ShaderConfiguration configuration;
		if (shaderType & SystemShaders::PhysicsForwardShader_Texture)
			configuration.defines["TEXTURE"] = "0";
		if (shaderType & SystemShaders::PhysicsForwardShader_GType_IBL) 
			configuration.defines["IBL"] = "0";
		if (shadowType != SystemShaders::ForwardShaderShadowType::None)
		{
			if (shadowType == SystemShaders::ForwardShaderShadowType::DEF)
				configuration.defines["HAS_DEF_SAMPLER"] = "0";
			else
				configuration.defines["HAS_PCF_SAMPLER"] = "0";
			configuration.defines["SHADOWMAP_TEXTURE"] = "0";
		}
		switch (lightType)
		{
		case SystemShaders::ForwardShaderLightType::Point:
			configuration.defines["IS_POINT_LIGHT"] = "TRUE";
			break;
		case SystemShaders::ForwardShaderLightType::Spot:
			configuration.defines["IS_SPOT_LIGHT"] = "TRUE";
			break;
		case SystemShaders::ForwardShaderLightType::Directional:
			configuration.defines["IS_DIRECTIONAL_LIGHT"] = "TRUE";
			break;
		default: // void type
			;
		}
		if (base)
			configuration.defines["IS_BASE"] = "TRUE";
		else
			configuration.defines["IS_ADD"] = "TRUE";
		configuration.version = "4_0";
		auto vs = sm.LoadShader<VertexShader>("PhysicsMaterialF.hlsl", configuration);
		auto ps = sm.LoadShader<PixelShader>("PhysicsMaterialF.hlsl", configuration);
		return Shaders(vs, ps);
	}

	Shaders GetForwardLambertShaders(bool base, int shaderType, SystemShaders::ForwardShaderLightType lightType,
		SystemShaders::ForwardShaderShadowType shadowType)
	{
		auto & sm = Context::Current().QueryShaderManager();
		ShaderConfiguration configuration;
		if (shaderType & SystemShaders::LambertForwardShader_Texture)
			configuration.defines["TEXTURE"] = "0";
		if (shadowType != SystemShaders::ForwardShaderShadowType::None)
		{
			if (shadowType == SystemShaders::ForwardShaderShadowType::DEF)
				configuration.defines["HAS_DEF_SAMPLER"] = "0";
			else
				configuration.defines["HAS_PCF_SAMPLER"] = "0";
			configuration.defines["SHADOWMAP_TEXTURE"] = "0";
		}
		switch (lightType)
		{
		case SystemShaders::ForwardShaderLightType::Point:
			configuration.defines["IS_POINT_LIGHT"] = "TRUE";
			break;
		case SystemShaders::ForwardShaderLightType::Spot:
			configuration.defines["IS_SPOT_LIGHT"] = "TRUE";
			break;
		case SystemShaders::ForwardShaderLightType::Directional:
			configuration.defines["IS_DIRECTIONAL_LIGHT"] = "TRUE";
			break;
		default: // void type
			;
		}
		if (base)
			configuration.defines["IS_BASE"] = "TRUE";
		else
			configuration.defines["IS_ADD"] = "TRUE";
		configuration.version = "4_0";
		auto vs = sm.LoadShader<VertexShader>("LambertMaterialF.hlsl", configuration);
		auto ps = sm.LoadShader<PixelShader>("LambertMaterialF.hlsl", configuration);
		return Shaders(vs, ps);
	}
	Shaders SystemShaders::GetForwardBasePhysicsShaders(int shaderType, ForwardShaderLightType lightType, ForwardShaderShadowType shadowType)
	{
		return GetForwardPhysicsShaders(true, shaderType, lightType, shadowType);
	}
	Shaders SystemShaders::GetForwardAddPhysicsShaders(int shaderType, ForwardShaderLightType lightType, ForwardShaderShadowType shadowType)
	{
		return GetForwardPhysicsShaders(false, shaderType, lightType, shadowType);
	}
	Shaders SystemShaders::GetForwardBaseLambertShaders(int shaderType, ForwardShaderLightType lightType, ForwardShaderShadowType shadowType)
	{
		return GetForwardLambertShaders(true, shaderType, lightType, shadowType);
	}

	Shaders SystemShaders::GetForwardAddLambertShaders(int shaderType, ForwardShaderLightType lightType,
		ForwardShaderShadowType shadowType)
	{
		return GetForwardLambertShaders(false, shaderType, lightType, shadowType);
	}

	Shaders SystemShaders::GetShadowMapShaders()
	{
		auto & sm = Context::Current().QueryShaderManager();
		ShaderConfiguration configuration;
		configuration.version = "4_0";
		auto vs = sm.LoadShader<VertexShader>("ShadowMap.hlsl", configuration);
		//auto ps = sm.LoadShader<PixelShader>("ShadowMap.hlsl", configuration);
		return Shaders(vs, nullptr);
	}
}
