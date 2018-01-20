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
		virtual void Render(const XMMATRIX * matrix) override;
		virtual void OnFocus(bool isForce);
		virtual void GetInerBox(::XGF::Shape::Rectangle & rc);

		virtual void OnAddToContainer() override;
		virtual void OnRemoveFromContainer() override;

		virtual void OnMouseUp(const Event & ev) override;
		virtual void OnMouseMove(const Event & ev) override;
		virtual void OnMouseDown(const Event & ev) override;

		void GetInnerRectangle(::XGF::Shape::Rectangle & rc);

		EditText();
		EditText(int id, wchar_t * text = L"", Color textColor = Color(0.f,0.f,0.f,1.f));
		~EditText();
		void SetText(const wchar_t * text);
		void SetTextColor(Color & color);
	protected:
		virtual Shape * GetShape() const override
		{
			return (::XGF::Shape::RectangleB *)this;
		};
	private:
		Color mTextColor;
		TextInputProcessor mTextInputProcessor;
	};


}
