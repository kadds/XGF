#include "..\..\Include\EditText.hpp"
#include "..\..\Include\Scene.hpp"
#include "..\..\Include\XGFramework.hpp"
#include "../../Include/RectangleB.hpp"
namespace XGF
{
	EditText::EditText():Control(), mTextColor(SM::Color(0.f, 0.f, 0.f, 1.f))
	{
		mTextInputProcessor.SetInnerRectangle(std::bind(&EditText::GetInnerRectangle, this, std::placeholders::_1));
		mTextInputProcessor.SetOnFocusListener(std::bind(&EditText::OnFocus, this, std::placeholders::_1));
	}

	EditText::EditText(int id, const string & text, const SM::Color & textColor):Control(id), mTextInputProcessor(text), mTextColor(textColor)
	{
		mTextInputProcessor.SetInnerRectangle(std::bind(&EditText::GetInnerRectangle, this, std::placeholders::_1));
		mTextInputProcessor.SetOnFocusListener(std::bind(&EditText::OnFocus, this, std::placeholders::_1));
	}

	EditText::~EditText()
	{
	}

	void EditText::SetText(const string & text)
	{
		mTextInputProcessor.SetText(text);
	}

	void EditText::SetTextColor(const SM::Color & color)
	{
		mTextColor = color;
	}

	void EditText::Render()
	{
		::XGF::Shape::Rectangle rc;
		RectangleB::GetInerBorderRectangle(rc);
		rc.SetZ(GetZ() - minZdivision);
		rc.mPolygon->ExpandAll(Operator::Multiply(GetMixMatrix()));
		DrawSkin(rc);
		mTextInputProcessor.SetTextRenderer(GetTextRenderer(this->mFontSize));

		mTextInputProcessor.RenderText(mTextColor);
	}
	void EditText::OnFocus(bool isForce)
	{
		if (isForce)
		{
			mNowState = SkinState::active;
		}
		else
		{
			mNowState = SkinState::normal;
		}
	}

	void EditText::GetInnerBox(::XGF::Shape::Rectangle & rc)
	{
		GetInerBorderRectangle(rc);
	}

	void EditText::OnAddToContainer()
	{
		Control::OnAddToContainer();
	}

	void EditText::OnRemoveFromContainer()
	{
		mParent->GetScene().GetFramework().GetInputManager().ClearFocus(&mTextInputProcessor);
		Control::OnRemoveFromContainer();
	}

	void EditText::OnMouseUp(const Event & ev)
	{
	}

	void EditText::OnMouseMove(const Event & ev)
	{
		if (!mParent->GetScene().GetFramework().GetInputManager().IsFocus(&mTextInputProcessor))
		{
			if(this->IsInBoundBox(Point((float)ev.GetDataInt(0), (float)ev.GetDataInt(1), 0.f), GetMixMatrix()))
				mNowState = SkinState::hover;
			else
				mNowState = SkinState::normal;
		}
		else
		{
			mNowState = SkinState::active;
		}
	}

	void EditText::OnMouseDown(const Event & ev)
	{
		if (this->IsInBoundBox(Point((float)ev.GetDataInt(0), (float)ev.GetDataInt(1), 0.f), GetMixMatrix()))
		{
			mParent->GetScene().GetFramework().GetInputManager().SetFocus(&mTextInputProcessor);
			mNowState = SkinState::active;
		}
	}

	void EditText::GetInnerRectangle(::XGF::Shape::Rectangle & rc)
	{
		GetInerBorderRectangle(rc);
		rc.SetZ(GetZ() - minZdivision);
	}

}
