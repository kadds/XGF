#include "../../Include/Label.hpp"
#include "../../Include/TextRenderer.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/UIBatches.hpp"
#include "../../Include/Container.hpp"
namespace XGF
{
	Label::Label()
	{
	}

	Label::~Label()
	{
	}

	void Label::Render(const XMMATRIX * matrix)
	{
		this->mParent->GetScene()->GetFramework()->GetUIBatches().GetTextRenderer(BATCHES_TEXTRENDERER_DEFAULT_SIZE)->DrawString(mText, Color(1,1,1,1), this, matrix);
	}

	void Label::SetText(wchar_t * text)
	{
		mText = text;
	}
}

