#include "..\..\..\Include\Geometry\BoxGeometry.hpp"
#include "../../../Include/Polygon.hpp"
namespace XGF::Shape
{
	void BoxGeometry::Init(float x, float y, float z)
	{
		mPolygonPleNormal = std::make_shared<PolygonPleNormalBinder>(mPolygon->GetActualCount());

		float hx = x / 2.f, hy = y / 2.f, hz = z / 2.f;
		auto * point = mPolygon->GetData();
		auto * normal = mPolygonPleNormal->GetData();

		// 1 Negative z-axis
		*(point++) = { hx, hy, -hz };
		*(point++) = { hx, -hy, -hz };
		*(point++) = { -hx, -hy, -hz };
		*(point++) = { -hx, hy, -hz };
		(normal++)->Set(0, 0, -1);
		(normal++)->Set(0, 0, -1);
		(normal++)->Set(0, 0, -1);
		(normal++)->Set(0, 0, -1);
		// 2 Negative y-axis
		*(point++) = { hx, -hy, -hz };
		*(point++) = { hx, -hy, hz };
		*(point++) = { -hx, -hy, hz };
		*(point++) = { -hx, -hy, -hz };
		(normal++)->Set(0, -1, 0);
		(normal++)->Set(0, -1, 0);
		(normal++)->Set(0, -1, 0);
		(normal++)->Set(0, -1, 0);

		// 3 Negative x-axis
		*(point++) = { -hx, -hy, -hz };
		*(point++) = { -hx, -hy, hz };
		*(point++) = { -hx, hy, hz };
		*(point++) = { -hx, hy, -hz };
		(normal++)->Set(-1, 0, 0);
		(normal++)->Set(-1, 0, 0);
		(normal++)->Set(-1, 0, 0);
		(normal++)->Set(-1, 0, 0);


		// 4 Z axis
		*(point++) = { hx, hy, hz };
		*(point++) = { hx, -hy, hz };
		*(point++) = { -hx, -hy, hz };
		*(point++) = { -hx, hy, hz };
		(normal++)->Set(0, 0, 1);
		(normal++)->Set(0, 0, 1);
		(normal++)->Set(0, 0, 1);
		(normal++)->Set(0, 0, 1);

		// 5 Y axis
		*(point++) = { hx, hy, -hz };
		*(point++) = { hx, hy, hz };
		*(point++) = { -hx, hy, hz };
		*(point++) = { -hx, hy, -hz };
		(normal++)->Set(0, 1, 0);
		(normal++)->Set(0, 1, 0);
		(normal++)->Set(0, 1, 0);
		(normal++)->Set(0, 1, 0);

		// 6 X axis
		*(point++) = { hx, -hy, -hz };
		*(point++) = { hx, -hy, hz };
		*(point++) = { hx, hy, hz };
		*(point++) = { hx, hy, -hz };
		(normal++)->Set(1, 0, 0);
		(normal++)->Set(1, 0, 0);
		(normal++)->Set(1, 0, 0);
		(normal++)->Set(1, 0, 0);

		InitializeIndex();
	}

	void BoxGeometry::InitializeIndex()
	{
		Index ix[] = {
			0, 1, 2, 2, 3, 0, //face 0
			4, 5, 6, 6, 7, 4, //face 1
			8, 9, 10, 10, 11, 8, //face 2
			14, 13, 12, 12, 15, 14,
			18, 17, 16, 16, 19, 18,
			22, 21, 20, 20, 23, 22
		};

		memcpy_s(&mPolygonPleIndex->Get(0), sizeof(ix), ix, sizeof(ix));
	}
}

