#include "..\..\Include\Cube.hpp"

namespace Shape {
Cube::Cube():Shape(8,36)
{
	index id[] = {
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7 };
	for (int i = 0; i < mPolygonPleIndex.mCount; i++)
	{
		mPolygonPleIndex.mIndex[i] = id[i];
	}
	

	mPolygon.mPoint[0] = Point(-1.0f, -1.0f, -1.0f);
	mPolygon.mPoint[1] = Point(-1.0f, 1.0f, -1.0f);
	mPolygon.mPoint[2] = Point(1.0f, 1.0f, -1.0f);
	mPolygon.mPoint[3] = Point(1.0f, -1.0f, -1.0f);
	mPolygon.mPoint[4] = Point(-1.0f, -1.0f, 1.0f);
	mPolygon.mPoint[5] = Point(-1.0f, 1.0f, 1.0f);
	mPolygon.mPoint[6] = Point(1.0f, 1.0f, 1.0f);
	mPolygon.mPoint[7] = Point(1.0f, -1.0f, 1.0f);
}


Cube::~Cube()
{
}
void Cube::SetPositionAndSize(float x, float y, float z, float width, float height, float depth)
{
	mPolygon.mPoint[0] = Point(x, y, z);
	mPolygon.mPoint[1] = Point(x, y + height, z);
	mPolygon.mPoint[2] = Point(x + width, y + height, z);
	mPolygon.mPoint[3] = Point(x + width, y , z );
	mPolygon.mPoint[4] = Point(x , y, z + depth);
	mPolygon.mPoint[5] = Point(x , y + height, z + depth);
	mPolygon.mPoint[6] = Point(x + width, y + height, z + depth);
	mPolygon.mPoint[7] = Point(x + width, y , z + depth);
}
void Cube::SetPosition(const Point & p)
{
	Point c;
	GetPosition(c);
	mPolygon.Translation(c.x + p.x, c.y + p.y, c.z + p.z);
}
void Cube::Filp()
{
	float temp;
	for (int i = 0; i < mPolygonPleIndex.mCount; i+=3)
	{
		temp = mPolygonPleIndex.mIndex[i + 1];
		mPolygonPleIndex.mIndex[i + 1] = mPolygonPleIndex.mIndex[i + 2];
		mPolygonPleIndex.mIndex[i + 2] = temp;
	}
}
}
