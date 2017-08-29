#pragma once
#include "Shape.hpp"
namespace Shape{
class Circle :
	public Shape
{
public:
	//precision：精度(三角形个数)
	Circle(int precision);
	~Circle();
	void SetPosiotionAndRadius(float x, float y, float r);
	virtual bool IsInBBox(const Point & p, const XMFLOAT4X4 * matrix) const;
	virtual void SetPosition(const Point & p);
	//always return 2
	int GetLayer(int layer[]);
private:
	float mRadius;
};
}
#define GetPrecision(radius,arclen) DirectX::XM_PI/arclen * radius 

