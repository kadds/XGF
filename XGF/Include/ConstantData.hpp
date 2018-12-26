#pragma once
#include "Defines.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
namespace XGF
{
	/*
	µ¥ÀýÀà
	*/
	class ConstantData
	{
	public:
		ConstantData() {};
		~ConstantData() {};
		void Initialize(GDI * gdi);//¶¥²ã´°¿Ú
		void Shutdown();

		Shaders GetPositionShader() { return mShaders[0]; }
		Shaders GetPositionTextureShader() { return mShaders[1]; }
		Shaders GetPositionColorShader() { return mShaders[2]; }
		Shaders GetPositionNormalShader() { return mShaders[3]; }
		Shaders GetPositionTextureColorShader() { return mShaders[4]; }
		Shaders GetPositionAlphaTextureColorShader() { return mShaders[5]; }
		Shaders GetPositionNormalTextureShader() { return mShaders[6]; }
		Shaders GetPositionNormalColorShader() { return mShaders[7]; }
		Shaders GetPositionNormalTextureColorShader() { return mShaders[8]; }

		static ConstantData & GetInstance() {
			return mConstantData;
		};
		ID3D11ShaderResourceView * GetRandomSRV() { return mRandomSRV; };
	private:
		static const int mMaxConstantShaderCount = 9;
		static const std::string mFileNames[mMaxConstantShaderCount];
		VertexShader mVShader[mMaxConstantShaderCount];
		PixelShader mPShader[mMaxConstantShaderCount];

		Shaders mShaders[mMaxConstantShaderCount];

		ID3D11ShaderResourceView * mRandomSRV;
		static ConstantData mConstantData;
		static UINT mRanowmSize;
	};
}


