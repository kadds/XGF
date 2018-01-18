#include "..\..\Include\TextColorButton.hpp"

namespace XGF
{
	TextColorButton::TextColorButton():mBackGroudColor(Color(0,0,0,1))
	{
		SetBorderSize(2);
		mBackGroudColor = Color(0.2, 0.2, 0.44, 1.0);
	}


	TextColorButton::~TextColorButton()
	{
	}

	void TextColorButton::Render(const XMMATRIX * matrix)
	{
	}

	void TextColorButton::SetText(wchar_t * text)
	{
		mText = text;
	}

}

