#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "TextInputProcessor.hpp"
#include "RectangleB.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	class EditText :
		public Control, public Shape::RectangleB
	{
	public:
		virtual void Render() override;
		virtual void OnFocus(bool isForce);
		virtual void GetInnerBox(::XGF::Shape::Rectangle & rc);

		virtual void OnAddToContainer() override;
		virtual void OnRemoveFromContainer() override;

		virtual void OnMouseUp(const Event & ev) override;
		virtual void OnMouseMove(const Event & ev) override;
		virtual void OnMouseDown(const Event & ev) override;

		void GetInnerRectangle(::XGF::Shape::Rectangle & rc);

		EditText();
		EditText(int id, const string & text = string(), const SM::Color & textColor = SM::Color(0.f,0.f,0.f,1.f));
		~EditText();
		void SetText(const string & text);
		void SetTextColor(const SM::Color & color);
	protected:
		virtual Shape * GetShape() const override
		{
			return (::XGF::Shape::RectangleB *)this;
		};
	private:
		SM::Color mTextColor;
		TextInputProcessor mTextInputProcessor;
	};


}
