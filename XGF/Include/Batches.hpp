#pragma once
#include "Defines.hpp"
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
		virtual Batch* GetBatch(int k);;
		virtual void Begin(WVPMatrix& wvp);;
		virtual void End();;
		virtual void Initialize(unsigned int CmaxVetices = 1024, unsigned int CmaxIndices = 1024 * 2,
		                        unsigned int TmaxVetices = 1024, unsigned int TmaxIndices = 1024 * 2);

		virtual void Shutdown();
		Batches();;
		virtual ~Batches();;
	};

}
