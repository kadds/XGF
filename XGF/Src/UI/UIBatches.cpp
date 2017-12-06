#include "..\..\Include\UIBatches.hpp"
#include "..\..\Include\ConstantData.hpp"
#include "..\..\Include\ShaderConst.hpp"
namespace XGF
{
	const int BATCHES_TEXTRENDERER_DEFAULT_SIZE = 5;
	const int BATCHES_TEXTRENDERER_SMALL_SIZE = 4;
	const int BATCHES_TEXTRENDERER_LARGE_SIZE = 6;
	const int BATCHES_TEXTRENDERER_LARGEER_SIZE = 7;
	const int BATCHES_TEXTRENDERER_HUGE_SIZE = 8;
	const int BATCHES_TEXTRENDERER_HUGEER_SIZE = 9;
	const int BATCHES_TEXTRENDERER_SMALLER_SIZE = 3;
	const int BATCHES_TEXTRENDERER_TINY_SIZE = 2;



	UIBatches::UIBatches()
	{
	}


	UIBatches::~UIBatches()
	{
	}

	void UIBatches::Begin(WVPMatrix & wvp)
	{
		Batches::Begin(wvp);
		for each(auto tx in maps)
			tx.second->Begin(wvp);
	}

	void UIBatches::End()
	{
		Batches::End();
		for each(auto tx in maps)
			tx.second->End();
	}

}
