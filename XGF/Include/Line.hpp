#pragma once
#include "Shape.hpp"
namespace Shape{
class Line :
	public Shape
{
public:
	Line();
	virtual ~Line();
	virtual void SetPosition(const Point & p) override;
	void SetEndPosition(Point p);
	virtual void GetPosition(Point & p) const override;
private:
	virtual bool IsInBBox(const Point & p, const XMFLOAT4X4 * matrix) const { return false; };
};

}

