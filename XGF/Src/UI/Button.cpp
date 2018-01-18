#include "../../Include/Button.hpp"
#include "../../Include/Batch.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/UIBatches.hpp"
namespace XGF 
{
	Button::Button()
	{
		
	}

	Button::~Button()
	{
	}
	
	void Button::Render(const XMMATRIX * matrix)
	{
		PolygonPleTextureBinder textureBinder(4);
		BindingBridge bbrige;
		textureBinder.FromTexture(&mSkin->GetTexture(mNowState));
		bbrige.AddBinder(textureBinder);
		Shape::Render(*(this->mParent->GetScene()->GetFramework()->GetUIBatches().GetBatch(BATCHES_BATCH_DEFAULT_PT)), matrix, bbrige, mSkin->GetTexture(mNowState));
	}

}
