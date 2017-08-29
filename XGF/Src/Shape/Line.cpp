#include "..\..\Include\Line.hpp"


namespace Shape {
Line::Line():Shape(2,2)
{
	mPolygonPleIndex.mIndex[0] = 0;
	mPolygonPleIndex.mIndex[1] = 1;
}


Line::~Line()
{
}
void Line::SetPosition(const Point & p)
{
	mPolygon.mPoint[0] = p;
}
void Line::SetEndPosition(Point p)
{
	mPolygon.mPoint[1] = p;
}
void Line::GetPosition(Point & p) const
{
	p = mPolygon.mPoint[0];
}
}