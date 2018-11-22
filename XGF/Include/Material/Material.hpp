#pragma once
#include "./../Shader.hpp"
#include "./../Polygon.hpp"

namespace XGF 
{
	namespace Shape
	{
		class Geometry;

		class Material
		{
		public:
			virtual ~Material() {};

			Shaders GetShaders();
			RasterizerState GetRasterizerState()
			{
				return mRasterizerState;
			};
			DepthStencilState GetDepthStencilState()
			{
				return mDepthStencilState;
			}
			BlendState GetBlendState()
			{
				return mBlendState;
			}
			void SetRasterizerState(RasterizerState rs)
			{
				if (rs != RasterizerState::InvalidValue)
					mRasterizerState = rs;
			};
			void SetDepthStencilState(DepthStencilState ds)
			{
				if (ds != DepthStencilState::InvalidValue)
					mDepthStencilState = ds;
			};
			void SetBlendState(BlendState bs)
			{
				if (bs != BlendState::InvalidValue)
					mBlendState = bs;
			}
			virtual std::vector<void* > GetConstantBuffers() = 0;
			virtual std::vector<Texture *> GetTextureBindingList() const = 0;
		protected:
			RasterizerState mRasterizerState = RasterizerState::SolidAndCutBack;
			DepthStencilState mDepthStencilState = DepthStencilState::DepthEnable;
			BlendState mBlendState = BlendState::NoneBlend;
			Shaders mShaders;

		};
	}
}