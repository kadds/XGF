#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "Texture.hpp"
#include "RectangleB.hpp"
namespace XGF
{
	class Button :
		public Control, public Shape::RectangleB
	{
	public:
		Button();
		Button(int id, wchar_t * text = L"", Color textColor = Color(0.f,0.f,0.f,1.f));
		~Button();
		virtual void Render(const XMMATRIX * matrix) override;

		void SetText(wchar_t * text);
		void SetTextColor(Color & color);
	protected:
		virtual Shape * GetShape() const override
		{
			return (::XGF::Shape::RectangleB *)(this);
		}
		Color mTextColor;
		wchar_t * mText;
	};
}


