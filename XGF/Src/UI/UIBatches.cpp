#include "..\..\Include\UIBatches.hpp"

#include "..\..\Include\ShaderConst.hpp"

void UIBatches::Initialize(GDI * gdi, Batches & batches, unsigned int CmaxVetices, unsigned int CmaxIndices, unsigned int TmaxVetices, unsigned int TmaxIndices)
{
	InputType inputType[2] = { SHADER_INPUTLAYOUT_POSITION,SHADER_INPUTLAYOUT_COLOR };
	mShaderPC.Initialize(gdi, ShaderConst::shaderPCVS, ShaderConst::shaderPCVSSize, ShaderConst::shaderPCPS, ShaderConst::shaderPCPSSize, inputType, 2);
	mBatchPC.Initialize(gdi, &mShaderPC,CmaxVetices, CmaxIndices);
	batches.insert(std::make_pair(BATCHES_BATCH_PC, &mBatchPC));
	
	InputType inputType2[2] = { SHADER_INPUTLAYOUT_POSITION,SHADER_INPUTLAYOUT_TEXTURE };
	mShaderPT.Initialize(gdi, ShaderConst::shaderPTVS, ShaderConst::shaderPTVSSize, ShaderConst::shaderPTPS, ShaderConst::shaderPTPSSize, inputType2, 2);
	mBatchPT.Initialize(gdi, &mShaderPT, TmaxVetices, TmaxVetices);
	batches.insert(std::make_pair(BATCHES_BATCH_PT, &mBatchPT));
	wvp = static_cast<WVPMatrix *>(_aligned_malloc(sizeof(WVPMatrix), 16));
	mBatchPC.SetBlend(true);
	mBatchPT.SetBlend(true);
}

void UIBatches::Shutdown(Batches & batches)
{
	batches.erase(BATCHES_BATCH_PC);
	batches.erase(BATCHES_BATCH_PT);
	mBatchPC.Shutdown();
	mBatchPT.Shutdown();
	mShaderPT.Shutdown();
	mShaderPC.Shutdown();
	_aligned_free(wvp);
}

void UIBatches::Begin()
{
	mCamera.GetCameraMatrix(*wvp);
	mBatchPC.Begin(*wvp);
	mBatchPT.Begin(*wvp);
}

void UIBatches::End()
{
	mBatchPC.End();
	mBatchPT.End();
}

void UIBatches::OnSize(int x, int y)
{
	mCamera.UpdataSize(x, y);
}


UIBatches::UIBatches()
{
}


UIBatches::~UIBatches()
{
}
