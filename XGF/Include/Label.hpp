#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	/*
	Display text
	*/
	class Label :
		public Control, public Shape::Rectangle
	{
	public:
		Label();
		Label(int id, const string & text = string(), SM::Color textColor = SM::Color(0.f,0.f,0.f,1.f));
		~Label();
		virtual void Render() override;
		void SetText(const string & text);
		void SetTextColor(const SM::Color & color);
	protected:
		virtual Shape * GetShape() const override
		{
			return (::XGF::Shape::Rectangle *)this;
		};
	private:
		string mText;
		SM::Color mTextColor;
	};


}
