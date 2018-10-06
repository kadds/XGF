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
			virtual BindingBridge CreateBinders(const Geometry & geometry) = 0;
		protected:
			RasterizerState mRasterizerState = RasterizerState::SolidAndCutBack;

			Shaders mShaders;
		};
	}
}