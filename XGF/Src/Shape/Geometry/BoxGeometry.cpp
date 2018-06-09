#include "..\..\..\Include\Geometry\BoxGeometry.hpp"

namespace XGF
{
	namespace Shape
	{
		BoxGeometry::BoxGeometry() :Geometry(4 * 6, 3 * 2 * 6)
		{
			InitializeIndex();
			this->SetSize(1, 1, 1);
		}
		BoxGeometry::BoxGeometry(float x, float y, float z) : Geometry(4 * 6, 3 * 2 * 6)
		{
			InitializeIndex();
			this->SetSize(x, y, z);
		}
		void BoxGeometry::SetSize(float x, float y, float z)
		{
			float hx = x / 2.f, hy = y / 2.f, hz = z / 2.f;
			Point * point = mPolygon.mPoint;

			// 1 ∏∫z÷·
			*(point++) = { hx, hy, -hz };
			*(point++) = { hx, -hy, -hz };
			*(point++) = { -hx, -hy, -hz };
			*(point++) = { -hx, hy, -hz };

			// 2 ∏∫y÷·
			*(point++) = { hx, -hy, -hz };
			*(point++) = { hx, -hy, hz };
			*(point++) = { -hx, -hy, hz };
			*(point++) = { -hx, -hy, -hz };
			// 3 ∏∫x÷·
			*(point++) = { -hx, -hy, -hz };
			*(point++) = { -hx, -hy, hz };
			*(point++) = { -hx, hy, hz };
			*(point++) = { -hx, hy, -hz };


			// 4 z÷·
			*(point++) = { hx, hy, hz };
			*(point++) = { hx, -hy, hz };
			*(point++) = { -hx, -hy, hz };
			*(point++) = { -hx, hy, hz };

			// 5 y÷·
			*(point++) = { hx, hy, -hz };
			*(point++) = { hx, hy, hz };
			*(point++) = { -hx, hy, hz };
			*(point++) = { -hx, hy, -hz };
			// 6 x÷·
			*(point++) = { hx, -hy, -hz };
			*(point++) = { hx, -hy, hz };
			*(point++) = { hx, hy, hz };
			*(point++) = { hx, hy, -hz };



		}

		void BoxGeometry::InitializeIndex()
		{
			index ix[] = {
				0, 1, 2, 2, 3, 0, //face 0
				4, 5, 6, 6, 7, 4, //face 1
				8, 9, 10, 10, 11, 8, //face 2
				14, 13, 12, 12, 15, 14,
				18, 17, 16, 16, 19, 18,
				22, 21, 20, 20, 23, 22
			};

			memcpy_s(mPolygonPleIndex.mIndex, sizeof(ix), ix, sizeof(ix));
		}

		BoxGeometry::~BoxGeometry()
		{
		}
	}
}

