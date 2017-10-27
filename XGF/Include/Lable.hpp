#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "Rectangle.hpp"
#include "TextRenderInterface.hpp"
/*
* �ؼ�����ǩ
* ��ʾһ���ı�
*/
class Lable :
	public Control, public TextRenderInterface, public Shape::Rectangle
{
public:
    Lable();
    ~Lable();
    virtual void Render(const XMMATRIX * matrix, const Batches & bs) override;
    void SetText(wchar_t * text);
    void SetColor(Color color) { mColor = color; }
protected:
	virtual Shape * GetShape() const override
	{
		return (::Shape::Rectangle *)this;
	};
private:
    void SetShape(ShapeType st) {};
	wchar_t * mText;
};
