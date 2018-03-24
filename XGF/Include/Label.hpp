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
		Label(int id, const string & text = string(), Color textColor = Color(0.f,0.f,0.f,1.f));
		~Label();
		virtual void Render() override;
		void SetText(const string & text);
		void SetTextColor(const Color & color);
	protected:
		virtual Shape * GetShape() const override
		{
			return (::XGF::Shape::Rectangle *)this;
		};
	private:
		string mText;
		Color mTextColor;
	};


}
