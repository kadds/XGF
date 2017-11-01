#include "../../Include/Lable.hpp"
#include "../../Include/TextRenderer.hpp"

Lable::Lable()
{
}

Lable::~Lable()
{
}

void Lable::Render(const XMMATRIX * matrix, const Batches & bs)
{
	mTextRenderer->DrawString(mText, mColor, this, matrix);
}

void Lable::SetText(wchar_t * text)
{
	mText = text;
	mIsChange = true;
}
