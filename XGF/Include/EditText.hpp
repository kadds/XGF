#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "TextInputInterface.hpp"
#include "RectangleB.hpp"
#include "Rectangle.hpp"

class EditText :
	public Control, public TextInputInterface, public Shape::RectangleB
{
public:
	EditText();
	~EditText();
	virtual void Render(const XMMATRIX * matrix, Batches & batches) override;
	virtual void OnMouseDowm(const MousePoint & mp, int c, bool isIn) override;
	virtual void OnMouseUp(const MousePoint & mp, int c, bool isIn) override;
	virtual void OnMouseMove(const MousePoint & mp, int cp, bool isIn) override;
	virtual void OnForce(bool isForce);
	virtual void GetInerBox(::Shape::Rectangle & rc);
	virtual TextRenderer * GetTextRenderer() override { return mTextRenderer; };
protected:
	virtual Shape * GetShape() const override
	{
		return (::Shape::RectangleB *)this;
	};
private:
	Color mBoderColor;
	Color mInerBoderColor;
	Color mbkColor;
	Color mTextColor;

	TextRenderer * mTextRenderer;
};

