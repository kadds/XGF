#include "..\..\Include\ConstantData.hpp"
#include "..\..\Include\ShaderConst.hpp"
namespace XGF
{
	ConstantData ConstantData::mConstantData;

	void ConstantData::Initialize(GDI * gdi)
	{
		ShaderLayout it[3] = { &SHADER_EL_POSITION3, &SHADER_EL_COLOR, &SHADER_EL_TEXTURE };
		mFontVShader.Initialize(gdi, ShaderConst::fontVS, ShaderConst::fontVSSize, it, 3);
		mFontPShader.Initialize(gdi, ShaderConst::fontPS, ShaderConst::fontPSSize);
		mFontShaders.Initialize(&mFontVShader, &mFontPShader, nullptr);
		it[1] = &SHADER_EL_TEXTURE;
		mPTVShader.Initialize(gdi, ShaderConst::shaderPTVS, ShaderConst::shaderPTVSSize, it, 2);
		mPTPShader.Initialize(gdi, ShaderConst::shaderPTPS, ShaderConst::shaderPTPSSize);
		mPTShaders.Initialize(&mPTVShader, &mPTPShader, nullptr);
		it[1] = &SHADER_EL_COLOR;
		mPCVShader.Initialize(gdi, ShaderConst::shaderPCVS, ShaderConst::shaderPCVSSize, it, 2);
		mPCPShader.Initialize(gdi, ShaderConst::shaderPCPS, ShaderConst::shaderPCPSSize);
		mPCShaders.Initialize(&mPCVShader, &mPCPShader, nullptr);
		it[1] = &SHADER_EL_COLOR;
		it[2] = &SHADER_EL_TEXTURE;
		mPCTVShader.Initialize(gdi, ShaderConst::shaderPCTVS, ShaderConst::shaderPCTVSSize, it, 3);
		mPCTPShader.Initialize(gdi, ShaderConst::shaderPCTPS, ShaderConst::shaderPCTPSSize);
		mPCTShaders.Initialize(&mPCTVShader, &mPCTPShader, nullptr);
	}

	void ConstantData::Shutdown()
	{
		mFontVShader.Shutdown();
		mFontPShader.Shutdown();
		mPTPShader.Shutdown();
		mPTVShader.Shutdown();
		mPCPShader.Shutdown();
		mPCVShader.Shutdown();
		mPCTPShader.Shutdown();
		mPCTVShader.Shutdown();
	}

}
