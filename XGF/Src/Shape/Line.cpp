#include "..\..\Include\Line.hpp"
#include "..\..\Include\Polygon.hpp"

namespace XGF
{
	namespace Shape
	{
		Line::Line() :Shape(2, 2)
		{
			mPolygonPleIndex->Get(0) = 0;
			mPolygonPleIndex->Get(1) = 1;
			mPolygon->SetFullActualCount();
			mPolygonPleIndex->SetFullActualCount();
		}


		Line::~Line()
		{
		}
		void Line::SetPosition(const Point & p)
		{
			mPolygon->GetData(0) = p;
		}
		void Line::SetEndPosition(Point p)
		{
			mPolygon->GetData(1) = p;
		}
		void Line::GetPosition(Point & p) const
		{
			p = mPolygon->GetData(0);
		}
	}
	
}