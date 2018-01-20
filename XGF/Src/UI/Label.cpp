#include "../../Include/Label.hpp"
#include "../../Include/TextRenderer.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/UIBatches.hpp"
#include "../../Include/Container.hpp"
namespace XGF
{
	Label::Label():mText(nullptr), mTextColor(Color(0.f, 0.f, 0.f, 1.f))
	{
	}

	Label::Label(int id, wchar_t * text, Color textColor):Control(id), mTextColor(textColor), mText(text)
	{
	}

	Label::~Label()
	{
	}

	void Label::Render(const XMMATRIX * matrix)
	{
		DrawSkin(matrix);
		auto renderer = this->mParent->GetScene()->GetFramework()->GetUIBatches().GetTextRenderer(mFontSize);
		if(renderer != nullptr)
			renderer->DrawString(mText, mTextColor, this, matrix);
	}

	void Label::SetText(wchar_t * text)
	{
		mText = text;
	}
	void Label::SetTextColor(Color & color)
	{
		mTextColor = color;
	}
}

