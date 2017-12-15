#pragma once
#include "Shader.hpp"
namespace XGF
{
	/*
	������
	*/
	class ConstantData
	{
	public:
		ConstantData() {};
		~ConstantData() {};
		void Initialize(GDI * gdi);//���㴰��
		void Shutdown();

		Shaders GetFontShaders() { return mFontShaders; }
		Shaders GetPTShaders() { return mPTShaders; }
		Shaders GetPCShaders() { return mPCShaders; }
		Shaders GetPCTShaders() { return mPCTShaders; }
		static ConstantData & GetInstance() {
			return mConstantData;
		};
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

		static ConstantData mConstantData;
	};
}


