#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "Texture.hpp"
#include "RectangleB.hpp"
#include <string>
namespace XGF
{
	class Button :
		public Control, public Shape::RectangleB
	{
	public:
		Button();
		Button(int id, const string & text = string(), SM::Color textColor = SM::Color(0.f,0.f,0.f,1.f));
		~Button();
		virtual void Render() override;

		void SetText(const string & text);
		void SetTextColor(const SM::Color & color);
	protected:
		virtual Shape * GetShape() const override
		{
			return (::XGF::Shape::RectangleB *)(this);
		}
		SM::Color mTextColor;
		std::wstring mText;
	};
}


