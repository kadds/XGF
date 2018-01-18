#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "TextInputInterface.hpp"
#include "RectangleB.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	class EditText :
		public Control, public TextInputInterface, public Shape::RectangleB
	{
	public:
		EditText();
		~EditText();
		virtual void Render(const XMMATRIX * matrix) override;
		virtual void OnForce(bool isForce);
		virtual void GetInerBox(::XGF::Shape::Rectangle & rc);
		virtual TextRenderer * GetTextRenderer() override { return mTextRenderer; };

		virtual void OnAddToContainer() override;
		virtual void OnRemoveFromContainer() override;

		virtual void OnMouseDown(const Event & ev) override;
	protected:
		virtual Shape * GetShape() const override
		{
			return (::XGF::Shape::RectangleB *)this;
		};
	private:
		Color mBoderColor;
		Color mInerBoderColor;
		Color mbkColor;
		Color mTextColor;

		TextRenderer * mTextRenderer;
	};


}
