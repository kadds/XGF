#include "../../Include/Shape.hpp"
#include "../../Include/Polygon.hpp"

namespace XGF
{
	namespace Shape
	{
		Shape::Shape(int n, int indexCount) 
		{
			mPolygon = std::make_shared<PolygonPlePointBinder>(n);
			mPolygonPleIndex = std::make_shared<PolygonPleIndex>(indexCount);
		}
		Shape::~Shape()
		{
		}
		void Shape::GetPosition(Point & p) const
		{
			p = mPolygon->GetData(0);
		}
		std::shared_ptr<PolygonPleIndex> Shape::GetIndex() const
		{
			return mPolygonPleIndex;
		}

		void Shape::SetZ(float z)
		{
			for (int i = 0; i < mPolygon->GetActualCount(); i++)
			{
				mPolygon->GetData(i).z = z;
			}
		}

		float Shape::GetZ()
		{
			return mPolygon->GetData()[0].z;
		}

		void Shape::Reset(int n, int indexCount)
		{
			mPolygon = std::make_shared<PolygonPlePointBinder>(n);
			mPolygonPleIndex = std::make_shared<PolygonPleIndex>(indexCount);
			// TODO: reset buffer
		}

		float triangleArea(Point a, Point b, Point c)
		{
			float result = std::abs((a.x * b.y + b.x * c.y + c.x * a.y - b.x * a.y
				- c.x * b.y - a.x * c.y) / 2.0f);
			return result;
		}
		// Determine whether the 2-dimensional point is inside the primitive
		// Use the PNPoly algorithm
		bool pInPolygon(const std::shared_ptr<PolygonPlePointBinder> & ql, float x, float y)
		{
			auto point = ql->GetData();
			// Generate a minimum bounding box
			Position minPosition = { point[0].x, point[0].y }, maxPosition = { point[0].x, point[0].y };
			for (int i = 1; i < ql->GetActualCount(); i++)
			{
				if (point[i].x < minPosition.x)
					minPosition.x = point[i].x;
				if (point[i].y < minPosition.y)
					minPosition.y = point[i].y;
				if (point[i].x > maxPosition.x)
					maxPosition.x = point[i].x;
				if (point[i].y > maxPosition.y)
					maxPosition.y = point[i].y;
			}

			if (x < minPosition.x || x > maxPosition.x || y < minPosition.y || y > maxPosition.y)
				return false;
			bool k = false;
			int i = 0;
			int j = ql->GetActualCount() - 1;
			for (; i < ql->GetActualCount(); j = i++) {
				if (((point[i].y > y) != (point[j].y > y)) &&
					(x < (point[j].x - point[i].x) * (y - point[i].y) /
					(point[j].y - point[i].y) + point[i].x))
					k = !k;
			}
			return k;
		}
	}
}
	
