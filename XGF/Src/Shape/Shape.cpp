#include "../../Include/Shape.hpp"
#include "../../Include/Batch.hpp"
#include "../../Include/Texture.hpp"
namespace XGF
{
	namespace Shape
	{
		Shape::Shape(int n, int indexCount) 
		{
			mPolygon = std::make_shared<PolygonPlePoint3>(n);
			mPolygonPleIndex = std::make_shared<PolygonPleIndex>(indexCount);
		}
		Shape::~Shape()
		{
		}
		void Shape::GetPosition(Point & p) const
		{
			p = mPolygon->mPoint[0];
		}
		std::shared_ptr<PolygonPleIndex> Shape::GetIndex() const
		{
			return mPolygonPleIndex;
		}

		void Shape::SetZ(float z)
		{
			for (int i = 0; i < mPolygon->mCount; i++)
			{
				mPolygon->mPoint[i].z = z;
			}
		}

		void Shape::Reset(int n, int indexCount)
		{
			mPolygon = std::make_shared<PolygonPlePoint3>(n);
			mPolygonPleIndex = std::make_shared<PolygonPleIndex>(indexCount);
			// TODO: reset buffer
		}

		float triangleArea(Point a, Point b, Point c)
		{
			float result = std::abs((a.x * b.y + b.x * c.y + c.x * a.y - b.x * a.y
				- c.x * b.y - a.x * c.y) / 2.0f);
			return result;
		}
		// 判断2维点是否在图元内部
		// 使用 PNPoly 算法
		bool pInPolygon(const std::shared_ptr<PolygonPlePoint3> & ql, float x, float y)
		{
			auto point = ql->mPoint;
			// 生成最小包围盒
			Position minPosition = { point[0].x, point[0].y }, maxPosition = { point[0].x, point[0].y };
			for (int i = 1; i < ql->mCount; i++) 
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
			int j = ql->mCount - 1;
			for (; i < ql->mCount; j = i++) {
				if (((point[i].y > y) != (point[j].y > y)) &&
					(x < (point[j].x - point[i].x) * (y - point[i].y) /
					(point[j].y - point[i].y) + point[i].x))
					k = !k;
			}
			return k;
		}
	}
}
	
