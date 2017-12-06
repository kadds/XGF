#include "..\..\Include\Circle.hpp"
#include <math.h>
namespace XGF
{
	namespace Shape
	{
		Circle::Circle(int precision) :Shape(precision + 1, precision * 3)
		{
			for (int i = 0; i < precision; i++)
			{
				mPolygonPleIndex.mIndex[i * 3] = 0;
				mPolygonPleIndex.mIndex[i * 3 + 1] = i + 1;
				mPolygonPleIndex.mIndex[i * 3 + 2] = i + 2;
			}
			mPolygonPleIndex.mIndex[precision * 3 - 1] = 1;
		}


		Circle::~Circle()
		{
		}
		void Circle::SetPosiotionAndRadius(float x, float y, float r)
		{
			mRadius = r;
			mPolygon.mPoint[0] = { x, y , 0.1f };
			float alpha;
			int precision = mPolygon.mCount - 1;
			float pie = DirectX::XM_2PI / precision;
			for (int i = 1; i < precision + 1; i++)
			{
				alpha = pie * i;
				float px, py;
				px = r * cos(alpha) + x;
				py = r * sin(alpha) + y;
				mPolygon.mPoint[i] = { px, py, 0.1f };
			}
		}
		bool Circle::IsInBBox(const Point & p, const XMFLOAT4X4 * matrix) const
		{
			float x = mPolygon.mPoint[0].x;
			float y = mPolygon.mPoint[0].y;
			if (matrix == nullptr)
			{
				return (p.x - x)*(p.x - x) + (p.y - y)*(p.y - y) <= mRadius * mRadius;
			}
			else
			{
				return false;//ÍµÁË¸öÀÁ
			}

		}
		void Circle::SetPosition(const Point & p)
		{
			SetPosiotionAndRadius(p.x, p.y, mRadius);
		}
		int Circle::GetLayer(int layer[])
		{
			layer[1] = mPolygon.mCount - 1;
			layer[0] = 1;
			return 2;
		}
	}
	
}