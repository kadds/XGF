#include "..\..\Include\ConstantData.hpp"
#include "..\..\Include\ShaderConst.hpp"
namespace XGF
{
	ConstantData ConstantData::mConstantData;

	void ConstantData::Initialize(GDI * gdi)
	{
		mFontVShader.Initialize(gdi, ShaderConst::fontVS, ShaderConst::fontVSSize);
		mFontPShader.Initialize(gdi, ShaderConst::fontPS, ShaderConst::fontPSSize);
		mFontShaders = { &mFontVShader, &mFontPShader , nullptr};

		mPTVShader.Initialize(gdi, ShaderConst::shaderPTVS, ShaderConst::shaderPTVSSize);
		mPTPShader.Initialize(gdi, ShaderConst::shaderPTPS, ShaderConst::shaderPTPSSize);
		mPTShaders = { &mPTVShader, &mPTPShader, nullptr };

		mPCVShader.Initialize(gdi, ShaderConst::shaderPCVS, ShaderConst::shaderPCVSSize);
		mPCPShader.Initialize(gdi, ShaderConst::shaderPCPS, ShaderConst::shaderPCPSSize);
		mPCShaders = { &mPCVShader, &mPCPShader, nullptr };

		mPCTVShader.Initialize(gdi, ShaderConst::shaderPCTVS, ShaderConst::shaderPCTVSSize);
		mPCTPShader.Initialize(gdi, ShaderConst::shaderPCTPS, ShaderConst::shaderPCTPSSize);
		mPCTShaders = { &mPCTVShader, &mPCTPShader, nullptr };
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
