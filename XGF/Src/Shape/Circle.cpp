#include "..\..\Include\Circle.hpp"
#include "../../Include/Polygon.hpp"

namespace XGF
{
	namespace Shape
	{
		Circle::Circle(int precision) :Shape2D(precision + 1, precision * 3)
		{
			for (int i = 0; i < precision; i++)
			{
				int ix = i * 3;
				mPolygonPleIndex->Get(ix) = 0;
				mPolygonPleIndex->Get(ix + 1) = i + 1;
				mPolygonPleIndex->Get(ix + 2) = i + 2;
			}
			mPolygonPleIndex->Get(precision * 3 - 1) = 1;
		}


		Circle::~Circle()
		{
		}
		void Circle::SetPositionAndRadius(float x, float y, float r)
		{
			mRadius = r;
			mPolygon->GetData(0) = { x, y , 0.1f };
			float alpha;
			int precision = mPolygon->GetActualCount() - 1;
			float pie = DirectX::XM_2PI / precision;
			for (int i = 1; i < precision + 1; i++)
			{
				alpha = pie * i;
				float px, py;
				px = r * cos(alpha) + x;
				py = r * sin(alpha) + y;
				mPolygon->GetData(i) = { px, py, 0.1f };
			}
		}
		bool Circle::IsInBoundBox(const Point & p, const SM::Matrix & matrix)
		{
			float x = mPolygon->GetData(0).x;
			float y = mPolygon->GetData(0).y;

				return (p.x - x)*(p.x - x) + (p.y - y)*(p.y - y) <= mRadius * mRadius;
			
				//return false; TODO:: incorrect when matrix != Identity
			

		}
		void Circle::SetPosition(const Point & p)
		{
			SetPositionAndRadius(p.x, p.y, mRadius);
		}
		int Circle::GetLayer(int layer[])
		{
			layer[1] = mPolygon->GetActualCount() - 1;
			layer[0] = 1;
			return 2;
		}
	}
	
}