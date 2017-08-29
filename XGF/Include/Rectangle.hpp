#pragma once
#include "Shape.hpp"

#define SHAPE_RECTANGLE 4
namespace Shape {
class Rectangle : public Shape
{
public:
    Rectangle();
	virtual ~Rectangle();
    void SetPositionAndSize(float x, float y, float width, float height);
	
	virtual bool IsInBBox(const Point & p, const XMFLOAT4X4 * matrix) const override;
    bool SubRectangle(Rectangle * pol, float x, float y, float width, float height) const;
	virtual void SetPosition(const Point & p);
	virtual void GetPosition(Point & p) const override;
private:
    
};
}

