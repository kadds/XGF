#include "..\..\Include\UIBatches.hpp"
#include "..\..\Include\ConstantData.hpp"
#include "..\..\Include\ShaderConst.hpp"

void UIBatches::Initialize(GDI * gdi, Batches & batches, unsigned int CmaxVetices, unsigned int CmaxIndices, unsigned int TmaxVetices, unsigned int TmaxIndices)
{
	mBatchPC.Initialize(gdi, ConstantData::GetInstance().GetPCShader(),CmaxVetices, CmaxIndices);
	batches.insert(std::make_pair(BATCHES_BATCH_PC, &mBatchPC));
	
	mBatchPT.Initialize(gdi, ConstantData::GetInstance().GetPTShader(), TmaxVetices, TmaxVetices);
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
