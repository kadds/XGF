#include "../../Include/Batches.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/ShaderManager.hpp"

namespace XGF
{
	const int BATCHES_BATCH_DEFAULT_PT = -1;
	const int BATCHES_BATCH_DEFAULT_PC = -2;

	Batch* Batches::GetBatch(int k)
	{
		if (k == BATCHES_BATCH_DEFAULT_PC)
			return &mBatchPC;
		if (k == BATCHES_BATCH_DEFAULT_PT)
			return &mBatchPT;
		return nullptr;
	}

	void Batches::Begin(WVPMatrix& wvp)
	{
		mBatchPC.GetShaderStage()->SetVSConstantBuffer(0, &wvp);
		mBatchPT.GetShaderStage()->SetVSConstantBuffer(0, &wvp);
		mBatchPC.GetShaderStage()->SetPSConstantBuffer(0, Color(1.f, 1.f, 1.f, 1.f));
		mBatchPT.GetShaderStage()->SetPSConstantBuffer(0, Color(1.f, 1.f, 1.f, 1.f));
		mBatchPC.Begin();
		mBatchPT.Begin();
	}

	void Batches::End()
	{
		mBatchPC.End();
		mBatchPT.End();
	}

	void Batches::Initialize(unsigned int CmaxVetices, unsigned int CmaxIndices, unsigned int TmaxVetices, unsigned int TmaxIndices)
	{
		mBatchPC.Initialize(Context::Current().QueryShaderManager().GetBasicShaders(false, false, true), CmaxVetices,
			CmaxIndices);
		mBatchPT.Initialize(Context::Current().QueryShaderManager().GetBasicShaders(false, true, false), TmaxVetices,
			TmaxIndices);
		mBatchPC.GetShaderStage()->SetBlendState(BlendState::AddOneOneAdd);
		mBatchPT.GetShaderStage()->SetBlendState(BlendState::AddOneOneAdd);
		mBatchPT.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthEnable);
		mBatchPC.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthEnable);
	}

	void Batches::Shutdown()
	{
		mBatchPC.Shutdown();
		mBatchPT.Shutdown();
	}

	Batches::Batches()
	{
	}

	Batches::~Batches()
	{
	}
}
