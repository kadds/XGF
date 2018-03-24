#include "../../Include/Label.hpp"
#include "../../Include/TextRenderer.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/UIBatches.hpp"
#include "../../Include/Container.hpp"
namespace XGF
{
	Label::Label(): mTextColor(Color(0.f, 0.f, 0.f, 1.f))
	{
	}

	Label::Label(int id, const string & text, Color textColor):Control(id), mTextColor(textColor), mText(text)
	{
	}

	Label::~Label()
	{
	}

	void Label::Render()
	{
		DrawSkin();
		auto renderer = this->mParent->GetScene()->GetFramework()->GetUIBatches().GetTextRenderer(mFontSize);
		if(renderer != nullptr)
			renderer->DrawString(mText.c_str(), mTextColor, this, &mTransform.GetMatrix());
	}

	void Label::SetText(const string & text)
	{
		mText = text;
	}
	void Label::SetTextColor(const Color & color)
	{
		mTextColor = color;
	}
}

