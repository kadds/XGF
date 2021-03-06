#include "..\..\Include\RectangleB.hpp"
#include "../../Include/Polygon.hpp"
namespace XGF
{
	namespace Shape
	{
		/*

		0-----------3
		|           |
		|4         7|
		|           |
		|           |
		|5         6|
		|           |
		1-----------2
		*/
		RectangleB::RectangleB() :Shape2D(10, 10 * 3)
		{
			Index ide[]{
				0,3,7,
				0,7,4,
				3,2,6,
				3,6,7,
				2,1,5,
				2,5,6,
				1,0,4,
				1,4,5,
				4,7,6,
				4,6,5
			};
			for (int i = 0; i < mPolygonPleIndex->GetActualCount(); i++)
			{
				mPolygonPleIndex->Get(i) = ide[i];
			}
			for (int i = 0; i < mPolygon->GetActualCount(); i++)
			{
				mPolygon->GetData(i) = Point(0.f, 0.f, 0.f);
			}
		}


		RectangleB::~RectangleB()
		{
		}

		void RectangleB::SetPositionAndSize(float x, float y, float width, float height)
		{
			auto shapePos = mPolygon->GetData();
			shapePos[0].x = shapePos[1].x = shapePos[4].x = shapePos[5].x = x;
			shapePos[0].y = shapePos[3].y = shapePos[4].y = shapePos[7].y = y;
			shapePos[2].x = shapePos[3].x = shapePos[7].x = shapePos[6].x = x + width;
			shapePos[1].y = shapePos[2].y = shapePos[5].y = shapePos[6].y = y + height;

			shapePos[0].z = shapePos[1].z = shapePos[2].z = shapePos[3].z = 0.0f;
			shapePos[4].z = shapePos[5].z = shapePos[6].z = shapePos[7].z = 0.0f;

		}

		void RectangleB::SetBorderSize(float width)
		{
			auto shapePos = mPolygon->GetData();
			shapePos[4].x = shapePos[5].x = shapePos[0].x + width;
			shapePos[7].x = shapePos[6].x = shapePos[2].x - width;
			shapePos[4].y = shapePos[7].y = shapePos[0].y + width;
			shapePos[5].y = shapePos[6].y = shapePos[2].y - width;
		}
		int RectangleB::GetBorderLayer(int layer[])
		{
			layer[0] = 4;
			layer[1] = 4;
			return 2;
		}
		void RectangleB::GetInerBorderRectangle(Rectangle & rc)
		{
			for (int i = 0; i < rc.mPolygon->GetActualCount(); i++)
			{
				rc.mPolygon->GetData(i) = mPolygon->GetData(4 + i);
			}
		}
		void RectangleB::SetPosition(const Point & p)
		{
			auto shapePos = mPolygon->GetData();
			shapePos[2].x = shapePos[3].x += p.x - shapePos[0].x;
			shapePos[1].y = shapePos[2].y += p.y - shapePos[0].y;
			shapePos[0].x = shapePos[1].x = p.x;
			shapePos[0].y = shapePos[3].y = p.y;
		}
		void RectangleB::GetPosition(Point & p) const
		{
			p = mPolygon->GetData(0);
		}
		bool RectangleB::IsInBoundBox(const Point & p, const SM::Matrix & matrix)
		{
			auto ple = std::make_shared<PolygonPlePointBinder>(4);

			for (int i = 0; i < 4; i++)
			{
				ple->GetData(i) = mPolygon->GetData(i);
			}
			ple->ExpandAll(Operator::Multiply(matrix));
			return pInPolygon(ple, p.x, p.y);

		}
	}


}
