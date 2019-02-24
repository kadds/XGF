#include "..\..\Include\Triangle.hpp"
#include "..\..\Include\Polygon.hpp"

namespace XGF
{
	namespace Shape
	{
		Triangle::Triangle() :Shape2D(3, 3)
		{
			mPolygonPleIndex->Get(0) = 0;
			mPolygonPleIndex->Get(1) = 1;
			mPolygonPleIndex->Get(2) = 2;
		}


		Triangle::~Triangle()
		{

		}
		void Triangle::SetThreePoint(const Point & p1, const Point & p2, const Point & p3)
		{
			mPolygon->GetData(0) = p1;
			mPolygon->GetData(1) = p2;
			mPolygon->GetData(2) = p3;

		}
		void Triangle::SetPosition(const Point & p)
		{
			auto shapePos = mPolygon->GetData();
			shapePos[2].x += p.x - shapePos[0].x;
			shapePos[1].y = shapePos[2].y += p.y - shapePos[0].y;
			shapePos[0].x = p.x;
			shapePos[0].y = p.y;
			shapePos[0].z = shapePos[1].z = shapePos[2].z = p.z;
		}
		void Triangle::GetPosition(Point & p) const
		{
			p = mPolygon->GetData(0);
		}
		bool Triangle::IsInBoundBox(const Point & p, const SM::Matrix & matrix)
		{
			auto ple = std::make_shared<PolygonPlePointBinder>(3);
			mPolygon->ExpandAllTo(*ple.get(), Operator::Multiply(matrix));
			return pInPolygon(ple, p.x, p.y);//TODO::EX
		}
	}
}