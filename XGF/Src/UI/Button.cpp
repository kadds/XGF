#include "../../Include/Button.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/UIBatches.hpp"
namespace XGF 
{
	Button::Button():mText(nullptr), mTextColor(SM::Color(0.f,0.f,0.f,1.f))
	{
	}

	Button::Button(int id, const string & text, SM::Color textColor):Control(id), mText(text), mTextColor(textColor)
	{
	}

	Button::~Button()
	{
	}
	
	void Button::Render()
	{
		DrawSkin();
		::XGF::Shape::Rectangle rc;
		RectangleB::GetInerBorderRectangle(rc);
		rc.SetZ(GetZ() - minZdivision);
		auto renderer = this->mParent->GetScene()->GetFramework()->GetUIBatches().GetTextRenderer(mFontSize);
		if (renderer != nullptr && !mText.empty())
			renderer->DrawString(mText.c_str(), mTextColor, &rc, &mTransform.GetMatrix());
	}

	void Button::SetText(const string & text)
	{
		mText = text;
	}
	void Button::SetTextColor(const SM::Color & color)
	{
		mTextColor = color;
	}
}
