#pragma once
#include "./../Texture.hpp"
#include "./../Shader.hpp"
namespace XGF 
{
	namespace Shape
	{
		class Material
		{
		public:
			struct BindDataReturns
			{
				int count;
				void * address;
			};
			Shaders GetShaders();
			RasterizerState GetRasterizerState()
			{
				return mRasterizerState;
			};
			void SetRasterizerState(RasterizerState rs)
			{
				if (rs != RasterizerState::InvalidValue)
					mRasterizerState = rs;
			};
		protected:
			RasterizerState mRasterizerState = RasterizerState::SolidAndCutBack;

			Shaders mShaders;
		};
	}
}