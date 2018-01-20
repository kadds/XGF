#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	/*
	* 控件：标签
	* 显示一段文本
	*/
	class Label :
		public Control, public Shape::Rectangle
	{
	public:
		Label();
		Label(int id, wchar_t * text = L"", Color textColor = Color(0.f,0.f,0.f,1.f));
		~Label();
		virtual void Render(const XMMATRIX * matrix) override;
		void SetText(wchar_t * text);
		void SetTextColor(Color & color);
	protected:
		virtual Shape * GetShape() const override
		{
			return (::XGF::Shape::Rectangle *)this;
		};
	private:
		wchar_t * mText;
		Color mTextColor;
	};


}
