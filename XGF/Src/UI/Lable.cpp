#include "../../Include/Lable.hpp"
#include "../../Include/TextRenderer.hpp"
#include "../../Include/UIBatches.hpp"
namespace XGF
{
	Lable::Lable()
	{
	}

	Lable::~Lable()
	{
	}

	void Lable::Render(const XMMATRIX * matrix, Batches & bs)
	{
		((UIBatches &)bs).GetTextRenderer(BATCHES_TEXTRENDERER_DEFAULT_SIZE)->DrawString(mText, mColor, this, matrix);
	}

	void Lable::SetText(wchar_t * text)
	{
		mText = text;
		mIsChange = true;
	}
}

