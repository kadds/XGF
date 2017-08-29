#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "TextRenderInterface.hpp"
#include "TextInputInterface.hpp"
#include "RectangleB.hpp"

class EditText :
	public Control, public TextInputInterface, public Shape::RectangleB
{
public:
	EditText();
	~EditText();
	virtual void Render(const XMMATRIX * matrix, const Batches & batches) override;
	virtual void OnMouseDowm(const MousePoint & mp, int c, bool isIn) override;
	virtual void OnMouseUp(const MousePoint & mp, int c, bool isIn) override;
	virtual void OnMouseMove(const MousePoint & mp, int cp, bool isIn) override;
	virtual void OnForce(bool isForce);
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
};

