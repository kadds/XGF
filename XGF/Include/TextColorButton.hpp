#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "RectangleB.hpp"
#include "UIBatches.hpp"
namespace XGF
{
	class TextColorButton :
		public Control, public Shape::RectangleB
	{
	public:
		TextColorButton();
		~TextColorButton();
		
		virtual void Render(const XMMATRIX * matrix) override;

		void SetText(wchar_t* text);
	private:
		virtual Shape * GetShape() const override
		{
			return (::XGF::Shape::RectangleB *)this;
		};
		wchar_t *mText;
		Color mBackGroudColor;
	};

}
