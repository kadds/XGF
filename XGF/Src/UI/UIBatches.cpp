#include "..\..\Include\UIBatches.hpp"
#include "..\..\Include\ConstantData.hpp"
#include "..\..\Include\ShaderConst.hpp"
namespace XGF
{

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

	TextRenderer * UIBatches::GetTextRenderer(FontSize size)
	{
		auto p = maps.find(size); 
		if (p != maps.end()) 
			return p->second; 
		return nullptr;
	}


}
