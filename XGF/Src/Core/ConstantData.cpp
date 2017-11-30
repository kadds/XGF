#include "..\..\Include\ConstantData.hpp"
#include "..\..\Include\ShaderConst.hpp"
ConstantData ConstantData::mConstantData;

void ConstantData::Initialize(GDI * gdi)
{
	ShaderLayout it[3] = { &SHADER_EL_POSITION3, &SHADER_EL_COLOR, &SHADER_EL_TEXTURE };
	mFontShader.Initialize(gdi, ShaderConst::fontVS, ShaderConst::fontVSSize, ShaderConst::fontPS, ShaderConst::fontPSSize, it, 3);
	it[1] = &SHADER_EL_TEXTURE;
	mPTShader.Initialize(gdi, ShaderConst::shaderPTVS, ShaderConst::shaderPTVSSize, ShaderConst::shaderPTPS, ShaderConst::shaderPTPSSize, it, 2);

	it[1] = &SHADER_EL_COLOR;
	mPCShader.Initialize(gdi, ShaderConst::shaderPCVS, ShaderConst::shaderPCVSSize, ShaderConst::shaderPCPS, ShaderConst::shaderPCPSSize, it, 2);
	it[1] = &SHADER_EL_COLOR;
	it[2] = &SHADER_EL_TEXTURE;
	mPCTShader.Initialize(gdi, ShaderConst::shaderPCTVS, ShaderConst::shaderPCTVSSize, ShaderConst::shaderPCTPS, ShaderConst::shaderPCTPSSize, it, 3);
}

void ConstantData::Shutdown()
{
	mFontShader.Shutdown();
	mPTShader.Shutdown();
	mPCShader.Shutdown();
	mPCTShader.Shutdown();
}
