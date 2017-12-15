#include "../../Include/Label.hpp"
#include "../../Include/TextRenderer.hpp"
#include "../../Include/UIBatches.hpp"
namespace XGF
{
	Label::Label()
	{
	}

	Label::~Label()
	{
	}

	void Label::Render(const XMMATRIX * matrix, Batches & bs)
	{
		((UIBatches &)bs).GetTextRenderer(BATCHES_TEXTRENDERER_DEFAULT_SIZE)->DrawString(mText, mColor, this, matrix);
	}

	void Label::SetText(wchar_t * text)
	{
		mText = text;
		mIsChange = true;
	}
}

