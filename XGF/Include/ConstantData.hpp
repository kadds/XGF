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

		Shaders GetFontShaders() { return mFontShaders; }
		Shaders GetPTShaders() { return mPTShaders; }
		Shaders GetPCShaders() { return mPCShaders; }
		Shaders GetPCTShaders() { return mPCTShaders; }
		static ConstantData & GetInstance() {
			return mConstantData;
		};
		ID3D11ShaderResourceView * GetRandomSRV() { return mRandomSRV; };
	private:
		VertexShader mFontVShader;
		PixelShader mFontPShader;
		VertexShader mPTVShader;
		PixelShader mPTPShader;
		VertexShader mPCVShader;
		PixelShader mPCPShader;
		VertexShader mPCTVShader;
		PixelShader mPCTPShader;

		Shaders mFontShaders;
		Shaders mPTShaders;
		Shaders mPCShaders;
		Shaders mPCTShaders;

		ID3D11ShaderResourceView * mRandomSRV;
		static ConstantData mConstantData;
		static UINT mRanowmSize;
	};
}


