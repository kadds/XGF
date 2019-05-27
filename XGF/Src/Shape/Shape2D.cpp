#include "..\..\Include\Shape2D.hpp"

namespace XGF
{
	namespace Shape
	{
		Shape2D::Shape2D(int n, int indexCount): Shape(n, indexCount)
		{
			mPolygon->SetFullActualCount();
			mPolygonPleIndex->SetFullActualCount();
		}


		Shape2D::~Shape2D()
		{
		}
	}
}


