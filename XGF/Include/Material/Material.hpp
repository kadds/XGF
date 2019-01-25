#pragma once
#include "./../Defines.hpp"
#include "./../Shader.hpp"
namespace XGF 
{
	namespace Shape
	{
		class Geometry;

		class Material
		{
		public:
			Material();;
			virtual ~Material();

			Shaders GetShaders();
			RasterizerState GetRasterizerState();
			DepthStencilState GetDepthStencilState();

			BlendState GetBlendState();

			void SetRasterizerState(RasterizerState rs);
			void SetDepthStencilState(DepthStencilState ds);
			void SetBlendState(BlendState bs);

			

			virtual std::vector<void*> GetVSBindingConstantBuffers();
			virtual std::vector<Texture *> GetVSBindingTextures();
			virtual std::vector<void*> GetPSBindingConstantBuffers();
			virtual std::vector<Texture *> GetPSBindingTextures();

			bool IsChangeShaders();
			void RemoveChangeShadersFlag();

			virtual  bool CanLinkWithLight()
			{
				return false;
			}
		protected:
			void SetShaders(Shaders shaders);
		private:
			RasterizerState mRasterizerState = RasterizerState::SolidAndCutBack;
			DepthStencilState mDepthStencilState = DepthStencilState::DepthEnable;
			BlendState mBlendState = BlendState::NoneBlend;
			Shaders mShaders;
			bool mIsChangeShaders;
		};
	}
}