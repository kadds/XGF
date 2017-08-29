#pragma once
class TextRenderer;
class TextRenderInterface
{
public :
	void SetTextRenderer(TextRenderer * tx)
	{
		mTextRenderer = tx;
	}
	TextRenderer * mTextRenderer;
};
