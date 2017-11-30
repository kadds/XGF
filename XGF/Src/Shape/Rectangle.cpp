#include "../../Include/Rectangle.hpp"
#include "../../Include/Control.hpp"
namespace Shape {
bool pInPolygon(const PolygonPlePoint3& ql, int x, int y);
Rectangle::Rectangle():Shape(4,6)
{
    mPolygonPleIndex.mIndex[0] = 0;
    mPolygonPleIndex.mIndex[1] = 3;
    mPolygonPleIndex.mIndex[2] = 2;
    mPolygonPleIndex.mIndex[3] = 2;
    mPolygonPleIndex.mIndex[4] = 1;
    mPolygonPleIndex.mIndex[5] = 0;
}


Rectangle::~Rectangle()
{

}

void Rectangle::SetPositionAndSize(float x, float y, float width, float height)
{
    auto shapePos = mPolygon.mPoint;
    shapePos[0].x = shapePos[1].x = x;
    shapePos[0].y = shapePos[3].y = y;
    shapePos[2].x = shapePos[3].x = x + width;
    shapePos[1].y = shapePos[2].y = y + height;
}


bool Rectangle::IsInBBox(const Point & p, const XMFLOAT4X4 * matrix) const
{
	if (matrix == nullptr)
	{
		auto shapePos = mPolygon.mPoint;
		return p.x >= shapePos[0].x && p.x <= shapePos[2].x && p.y >= shapePos[0].y && p.y <= shapePos[1].y;
	}
	else
	{
		PolygonPlePoint3 ple(4);
		mPolygon.MulTo(&ple, DirectX::XMLoadFloat4x4(matrix));
		return pInPolygon(ple, static_cast<int>(p.x), static_cast<int>(p.y));
	}
}

bool Rectangle::SubRectangle(Rectangle * pol, float x, float y, float width, float height) const
{
    pol->mPolygon.mPoint[0].x = pol->mPolygon.mPoint[1].x = mPolygon.mPoint[0].x + x;
    pol->mPolygon.mPoint[2].x = pol->mPolygon.mPoint[3].x = mPolygon.mPoint[0].x + x + width;

    pol->mPolygon.mPoint[0].y = pol->mPolygon.mPoint[3].y = mPolygon.mPoint[0].y + y;
    pol->mPolygon.mPoint[2].y = pol->mPolygon.mPoint[1].y = mPolygon.mPoint[0].y + y + height;
	pol->mPolygon.mPoint[3].z = mPolygon.mPoint[3].z;
	pol->mPolygon.mPoint[0].z = mPolygon.mPoint[0].z;
	pol->mPolygon.mPoint[2].z = mPolygon.mPoint[2].z;
	pol->mPolygon.mPoint[1].z = mPolygon.mPoint[1].z;
    return pol->mPolygon.mPoint[2].y <= mPolygon.mPoint[2].y && pol->mPolygon.mPoint[2].x <= mPolygon.mPoint[2].x;
}
void Rectangle::SetPosition(const Point & p)
{
	auto shapePos = mPolygon.mPoint;
	shapePos[2].x = shapePos[3].x += p.x -shapePos[0].x;
	shapePos[1].y = shapePos[2].y += p.y - shapePos[0].y;
	shapePos[0].x = shapePos[1].x = p.x;
	shapePos[0].y = shapePos[3].y = p.y;
	shapePos[0].z = shapePos[1].z = shapePos[2].z = shapePos[3].z = p.z;
}

void Rectangle::GetPosition(Point & p) const
{
	p = mPolygon.mPoint[0];
}

}
