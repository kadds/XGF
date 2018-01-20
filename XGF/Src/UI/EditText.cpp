#include "..\..\Include\EditText.hpp"
#include "..\..\Include\Scene.hpp"
#include "..\..\Include\XGFramework.hpp"
#include "..\..\Include\Batch.hpp"
#include "../../Include/UIBatches.hpp"
#include "../../Include/RectangleB.hpp"
namespace XGF
{
	EditText::EditText():Control(), mTextColor(Color(0.f, 0.f, 0.f, 1.f))
	{
		mTextInputProcessor.SetInnerRectangle(std::bind(&EditText::GetInnerRectangle, this, std::placeholders::_1));
		mTextInputProcessor.SetOnFocusListener(std::bind(&EditText::OnFocus, this, std::placeholders::_1));
	}

	EditText::EditText(int id, wchar_t * text, Color textColor):Control(id), mTextInputProcessor(text), mTextColor(textColor)
	{
		mTextInputProcessor.SetInnerRectangle(std::bind(&EditText::GetInnerRectangle, this, std::placeholders::_1));
		mTextInputProcessor.SetOnFocusListener(std::bind(&EditText::OnFocus, this, std::placeholders::_1));
	}

	EditText::~EditText()
	{
	}

	void EditText::SetText(const wchar_t * text)
	{
		mTextInputProcessor.SetText(text);
	}

	void EditText::SetTextColor(Color & color)
	{
		mTextColor = color;
	}

	void EditText::Render(const XMMATRIX * matrix)
	{
		DrawSkin(matrix);
		mTextInputProcessor.SetTextRenderer(GetTextRenderer(this->mFontSize));

		mTextInputProcessor.RenderText(matrix, mTextColor);
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

	void EditText::GetInerBox(::XGF::Shape::Rectangle & rc)
	{
		GetInerBorderRectangle(rc);
	}

	void EditText::OnAddToContainer()
	{
		Control::OnAddToContainer();
	}

	void EditText::OnRemoveFromContainer()
	{
		Control::OnRemoveFromContainer();
		mParent->GetScene()->GetFramework()->GetInputManager()->ClearFocus(&mTextInputProcessor);
	}

	void EditText::OnMouseUp(const Event & ev)
	{
	}

	void EditText::OnMouseMove(const Event & ev)
	{
		if (!mParent->GetScene()->GetFramework()->GetInputManager()->IsFocus(&mTextInputProcessor))
		{
			if(this->IsInBoundBox(Point(ev.GetDataInt(0), ev.GetDataInt(1), 0.f), mTransform.GetMatrix()))
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
		if (this->IsInBoundBox(Point(ev.GetDataInt(0), ev.GetDataInt(1), 0.f), mTransform.GetMatrix()))
		{
			mParent->GetScene()->GetFramework()->GetInputManager()->SetFocus(&mTextInputProcessor);
			mNowState = SkinState::active;
		}
	}

	void EditText::GetInnerRectangle(::XGF::Shape::Rectangle & rc)
	{
		GetInerBorderRectangle(rc);
		rc.SetZ(GetZ() - minZdivision);
	}

}
