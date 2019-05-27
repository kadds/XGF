#pragma once
#include "Defines.hpp"
#include "Shader.hpp"
#include "Shadow.hpp"

namespace XGF
{
	class SystemShaders
	{
	public:
		enum BasicShaderType
		{
			BasicShader_Nothing = 0,
			BasicShader_Normal = 1,
			BasicShader_Texture = 2,
			BasicShader_VertexColor = 4,
		};
		enum PhongForwardShaderType
		{
			PhongForwardShader_Nothing = 0,
			PhongForwardShader_BlinnPhong = 1,
			PhongForwardShader_Texture = 2,
		};
		enum LambertForwardShaderType
		{
			LambertForwardShader_Nothing = 0,
			LambertForwardShader_Texture = 1,
		};
		enum PhysicsForwardShaderType
		{
			PhysicsForwardShader_Nothing = 0,
			PhysicsForwardShader_Texture = 1,
			PhysicsForwardShader_GType_IBL = 2,
		};
		enum class ForwardShaderLightType
		{
			None,
			Point,
			Spot,
			Directional,
		};
		enum class ForwardShaderShadowType
		{
			None,
			DEF,
			PCF,
		};
		/**
		 * \brief Get shaders from basic material
		 * \param type BasicShader_XXX or with the and operator likes BasicShader_Normal & BasicShader_Texture
		 * \return Shaders as you expect
		 */
		static Shaders GetBasicShaders(int type);
		
		static Shaders GetFontShaders(bool hasVertexColor = false);
		
		static Shaders GetForwardBasePhongShaders(int shaderType, ForwardShaderLightType lightType, ForwardShaderShadowType shadowType);

		static Shaders GetForwardAddPhongShaders(int shaderType, ForwardShaderLightType lightType, ForwardShaderShadowType shadowType);

		static Shaders GetForwardBasePhysicsShaders(int shaderType, ForwardShaderLightType lightType, ForwardShaderShadowType shadowType);

		static Shaders GetForwardAddPhysicsShaders(int shaderType, ForwardShaderLightType lightType, ForwardShaderShadowType shadowType);

		static Shaders GetForwardBaseLambertShaders(int shaderType, ForwardShaderLightType lightType, ForwardShaderShadowType shadowType);
		static Shaders GetForwardAddLambertShaders(int shaderType, ForwardShaderLightType lightType, ForwardShaderShadowType shadowType);
		static Shaders GetImguiShaders();

		static Shaders GetShadowMapShaders();
		
	};
}
