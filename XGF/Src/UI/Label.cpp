#include "../../Include/Label.hpp"
#include "../../Include/TextRenderer.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/Container.hpp"
namespace XGF
{
	Label::Label(): mTextColor(SM::Color(0.f, 0.f, 0.f, 1.f))
	{
	}

	Label::Label(int id, const string & text, SM::Color textColor):Control(id), mTextColor(textColor), mText(text)
	{
	}

	Label::~Label()
	{
	}

	void Label::Render()
	{
		//DrawSkin();
		auto renderer = GetTextRenderer();
		if(renderer != nullptr)
			renderer->DrawString(mText.c_str(), mTextColor, this, &GetMixMatrix());
	}

	void Label::SetText(const string & text)
	{
		mText = text;
	}
	void Label::SetTextColor(const SM::Color & color)
	{
		mTextColor = color;
	}
}

