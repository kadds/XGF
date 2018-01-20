#include "../../Include/Button.hpp"
#include "../../Include/Batch.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/XGFramework.hpp"
#include "../../Include/UIBatches.hpp"
namespace XGF 
{
	Button::Button():mText(nullptr), mTextColor(Color(0.f,0.f,0.f,1.f))
	{
	}

	Button::Button(int id, wchar_t * text, Color textColor):Control(id), mText(text), mTextColor(textColor)
	{
	}

	Button::~Button()
	{
	}
	
	void Button::Render(const XMMATRIX * matrix)
	{
		DrawSkin(matrix);
		::XGF::Shape::Rectangle rc;
		RectangleB::GetInerBorderRectangle(rc);
		rc.SetZ(GetZ() - minZdivision);
		auto renderer = this->mParent->GetScene()->GetFramework()->GetUIBatches().GetTextRenderer(mFontSize);
		if (renderer != nullptr && mText != nullptr)
			renderer->DrawString(mText, mTextColor, &rc, matrix);
	}

	void Button::SetText(wchar_t * text)
	{
		mText = text;
	}
	void Button::SetTextColor(Color & color)
	{
		mTextColor = color;
	}
}
