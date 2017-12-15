#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	/*
	* �ؼ�����ǩ
	* ��ʾһ���ı�
	*/
	class Label :
		public Control, public Shape::Rectangle
	{
	public:
		Label();
		~Label();
		virtual void Render(const XMMATRIX * matrix, Batches & bs) override;
		void SetText(wchar_t * text);
		void SetColor(Color color) { mColor = color; }
	protected:
		virtual Shape * GetShape() const override
		{
			return (::XGF::Shape::Rectangle *)this;
		};
	private:
		void SetShape(::XGF::Shape::ShapeType st) {};
		wchar_t * mText;
	};


}
