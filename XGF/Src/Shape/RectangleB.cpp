#include "..\..\Include\RectangleB.hpp"

namespace XGF
{
	namespace Shape
	{
		/*

		0-----------3
		|           |
		|4         7|
		|  8     11 |
		|  9     10 |
		|5         6|
		|           |
		1-----------2
		*/
		RectangleB::RectangleB() :Shape(12, 10 * 3)
		{
			index ide[]{
				0,3,7,
				0,7,4,
				3,2,6,
				3,6,7,
				2,1,5,
				2,5,6,
				1,0,4,
				1,4,5,
				11,10,9,
				9,8,11
			};
			for (int i = 0; i < mPolygonPleIndex.mCount; i++)
			{
				mPolygonPleIndex.mIndex[i] = ide[i];
			}
		}


		RectangleB::~RectangleB()
		{
		}

		void RectangleB::SetPositionAndSize(float x, float y, float width, float height)
		{
			auto shapePos = mPolygon.mPoint;
			shapePos[8].x = shapePos[9].x = shapePos[0].x = shapePos[1].x = shapePos[4].x = shapePos[5].x = x;
			shapePos[8].y = shapePos[11].y = shapePos[0].y = shapePos[3].y = shapePos[4].y = shapePos[7].y = y;
			shapePos[10].x = shapePos[11].x = shapePos[2].x = shapePos[3].x = shapePos[7].x = shapePos[6].x = x + width;
			shapePos[10].y = shapePos[9].y = shapePos[1].y = shapePos[2].y = shapePos[5].y = shapePos[6].y = y + height;

			shapePos[0].z = shapePos[1].z = shapePos[2].z = shapePos[3].z = 0.0f;
			shapePos[4].z = shapePos[5].z = shapePos[6].z = shapePos[7].z = 0.0f;

			shapePos[8].z = shapePos[9].z = shapePos[10].z = shapePos[11].z = 0.0f;

		}

		void RectangleB::SetBorderSize(float width)
		{
			auto shapePos = mPolygon.mPoint;
			shapePos[8].x = shapePos[9].x = shapePos[4].x = shapePos[5].x = shapePos[0].x + width;
			shapePos[10].x = shapePos[11].x = shapePos[7].x = shapePos[6].x = shapePos[2].x - width;
			shapePos[8].y = shapePos[11].y = shapePos[4].y = shapePos[7].y = shapePos[0].y + width;
			shapePos[10].y = shapePos[9].y = shapePos[5].y = shapePos[6].y = shapePos[2].y - width;
		}
		bool RectangleB::IsInBBox(const Point & p, const XMFLOAT4X4 * matrix) const
		{
			if (matrix != nullptr)
			{
				PolygonPlePoint3 ple(4);

				for (int i = 0; i < 4; i++)
				{
					ple.mPoint[i] = mPolygon.mPoint[i];
				}
				ple.Mul(DirectX::XMLoadFloat4x4(matrix));
				return pInPolygon(ple, static_cast<int>(p.x), static_cast<int>(p.y));
			}
			else
			{
				auto shapePos = mPolygon.mPoint;
				return p.x >= shapePos[0].x && p.x <= shapePos[2].x && p.y >= shapePos[0].y && p.y <= shapePos[1].y;
			}
		}
		int RectangleB::GetBorderLayer(int layer[])
		{
			layer[0] = 4;
			layer[1] = 4;
			layer[2] = 4;
			return 3;
		}
		void RectangleB::GetInerBorderRectangle(Rectangle & rc)
		{
			for (int i = 0; i < rc.mPolygon.mCount; i++)
			{
				rc.mPolygon.mPoint[i] = mPolygon.mPoint[4 + i];
			}
		}
		void RectangleB::SetPosition(const Point & p)
		{
			auto shapePos = mPolygon.mPoint;
			shapePos[2].x = shapePos[3].x += p.x - shapePos[0].x;
			shapePos[1].y = shapePos[2].y += p.y - shapePos[0].y;
			shapePos[0].x = shapePos[1].x = p.x;
			shapePos[0].y = shapePos[3].y = p.y;
		}
		void RectangleB::GetPosition(Point & p) const
		{
			p = mPolygon.mPoint[0];
		}
	}


}
