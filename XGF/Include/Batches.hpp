#pragma once
#include "Defines.hpp"
#include "ConstantData.hpp"
#include "Batch.hpp"
namespace XGF
{
	class Batch;
	class TextRenderer;
	typedef void* RendererUnion;

	extern const int BATCHES_BATCH_DEFAULT_PT;
	extern const int BATCHES_BATCH_DEFAULT_PC;

	class Batches {
	protected:
		Batch mBatchPT;
		Batch mBatchPC;
	public:
		virtual Batch * GetBatch(int k) {
			if (k == BATCHES_BATCH_DEFAULT_PC)
				return &mBatchPC;
			if (k == BATCHES_BATCH_DEFAULT_PT)
				return &mBatchPT;
			return nullptr;
		};
		virtual void Begin(WVPMatrix & wvp) { mBatchPC.GetShaderStage()->SetVSConstantBuffer(0, &wvp);mBatchPT.GetShaderStage()->SetVSConstantBuffer(0, &wvp); mBatchPC.Begin(); mBatchPT.Begin(); };
		virtual void End() { mBatchPC.End();mBatchPT.End(); };
		virtual void Initialize(GDI * gdi, unsigned int CmaxVetices = 1024, unsigned int CmaxIndices = 1024 * 2, unsigned int TmaxVetices = 1024, unsigned int TmaxIndices = 1024 * 2)
		{
			mBatchPC.Initialize(gdi, ConstantData::GetInstance().GetPCShaders(), CmaxVetices, CmaxIndices);
			mBatchPT.Initialize(gdi, ConstantData::GetInstance().GetPTShaders(), TmaxVetices, TmaxIndices);
			mBatchPC.GetShaderStage()->SetBlendState(BlendState::AddOneOneAdd);
			mBatchPT.GetShaderStage()->SetBlendState(BlendState::AddOneOneAdd);
			mBatchPT.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthEnable);
			mBatchPC.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthEnable);
		}

		virtual void Shutdown()
		{
			mBatchPC.Shutdown();
			mBatchPT.Shutdown();
		}
		Batches() {};
		virtual ~Batches() {};
	};

}
