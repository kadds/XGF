#include "..\..\Include\Triangle.hpp"

namespace XGF
{
	namespace Shape
	{
		Triangle::Triangle() :Shape2D(3, 3)
		{
			mPolygonPleIndex->mIndex[0] = 0;
			mPolygonPleIndex->mIndex[1] = 1;
			mPolygonPleIndex->mIndex[2] = 2;
		}


		Triangle::~Triangle()
		{

		}
		void Triangle::SetThreePoint(Point & p1, Point & p2, Point & p3)
		{
			mPolygon->mPoint[0] = p1;
			mPolygon->mPoint[1] = p2;
			mPolygon->mPoint[2] = p3;
			//DirectX::XMLoadFloat3(&p1), DirectX::XMLoadFloat3(&p2), DirectX::XMLoadFloat3(&p3)\
			//不做逆时针方向判断
		}
		void Triangle::SetPosition(const Point & p)
		{
			auto shapePos = mPolygon->mPoint;
			shapePos[2].x += p.x - shapePos[0].x;
			shapePos[1].y = shapePos[2].y += p.y - shapePos[0].y;
			shapePos[0].x = p.x;
			shapePos[0].y = p.y;
			shapePos[0].z = shapePos[1].z = shapePos[2].z = p.z;
		}
		void Triangle::GetPosition(Point & p) const
		{
			p = mPolygon->mPoint[0];
		}
		bool Triangle::IsInBoundBox(const Point & p, const SM::Matrix & matrix)
		{
			auto ple = std::make_shared<PolygonPlePoint3>(3);
			mPolygon->MulTo(ple, matrix);
			return pInPolygon(ple, p.x, p.y);//TODO::EX
		}
	}
}