#include "..\..\Include\ConstantData.hpp"
#include "..\..\Include\ShaderConst.hpp"
ConstantData ConstantData::mConstantData;

void ConstantData::Initialize(GDI * gdi)
{
	InputType it[3] = { SHADER_INPUTLAYOUT_POSITION,SHADER_INPUTLAYOUT_COLOR,SHADER_INPUTLAYOUT_TEXTURE };
	mFontShader.Initialize(gdi, ShaderConst::fontVS, ShaderConst::fontVSSize, ShaderConst::fontPS, ShaderConst::fontPSSize, it, 3);
	it[1] = SHADER_INPUTLAYOUT_TEXTURE;
	mPTShader.Initialize(gdi, ShaderConst::shaderPTVS, ShaderConst::shaderPTVSSize, ShaderConst::shaderPTPS, ShaderConst::shaderPTPSSize, it, 2);

	it[1] = SHADER_INPUTLAYOUT_COLOR;
	mPCShader.Initialize(gdi, ShaderConst::shaderPCVS, ShaderConst::shaderPCVSSize, ShaderConst::shaderPCPS, ShaderConst::shaderPCPSSize, it, 2);
}

void ConstantData::Shutdown()
{
	mFontShader.Shutdown();
	mPTShader.Shutdown();
	mPCShader.Shutdown();
}
