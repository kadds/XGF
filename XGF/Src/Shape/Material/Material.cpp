#include "..\..\..\Include\Material\Material.hpp"

namespace XGF
{
	namespace Shape
	{
		Material::Material(): mIsChangeShaders(false)
		{
		}

		Material::~Material()
		{
		}

		Shaders Material::GetShaders()
		{
			return mShaders;
		}

		RasterizerState Material::GetRasterizerState()
		{
			return mRasterizerState;
		}

		DepthStencilState Material::GetDepthStencilState()
		{
			return mDepthStencilState;
		}

		BlendState Material::GetBlendState()
		{
			return mBlendState;
		}

		void Material::SetRasterizerState(RasterizerState rs)
		{
			if (rs != RasterizerState::InvalidValue)
				mRasterizerState = rs;
		}

		void Material::SetDepthStencilState(DepthStencilState ds)
		{
			if (ds != DepthStencilState::InvalidValue)
				mDepthStencilState = ds;
		}

		void Material::SetBlendState(BlendState bs)
		{
			if (bs != BlendState::InvalidValue)
				mBlendState = bs;
		}

		std::vector<void*> Material::GetVSBindingConstantBuffers()
		{
			return std::vector<void *>();
		}

		std::vector<Texture*> Material::GetVSBindingTextures()
		{
			return std::vector<Texture *>();
		}

		std::vector<void*> Material::GetPSBindingConstantBuffers()
		{
			return std::vector<void *>();
		}

		std::vector<Texture*> Material::GetPSBindingTextures()
		{
			return std::vector<Texture *>();
		}

		bool Material::IsChangeShaders()
		{
			return mIsChangeShaders;
		}

		void Material::RemoveChangeShadersFlag()
		{
			mIsChangeShaders = false;
		}

		void Material::SetShaders(Shaders shaders)
		{
			if (mShaders != shaders)
				mIsChangeShaders = true;
			mShaders = shaders;
		}
	}
}