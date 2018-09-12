#include "..\..\..\Include\Geometry\PlaneGeometry.hpp"

namespace XGF
{
	namespace Shape
	{
		PlaneGeometry::PlaneGeometry(float width, float height) : Geometry(4, 6)
		{
			auto halfWidth = width / 2;
			auto halfHeight = height / 2;
			mPolygon->mPoint[0] = {halfWidth, 0, halfHeight};
			mPolygon->mPoint[1] = { -halfWidth, 0, halfHeight };
			mPolygon->mPoint[2] = { -halfWidth, 0, -halfHeight };
			mPolygon->mPoint[3] = { halfWidth, 0, -halfHeight };

			mPolygonPleIndex->mIndex[0] = 2;
			mPolygonPleIndex->mIndex[1] = 1;
			mPolygonPleIndex->mIndex[2] = 0;
			mPolygonPleIndex->mIndex[3] = 0;
			mPolygonPleIndex->mIndex[4] = 3;
			mPolygonPleIndex->mIndex[5] = 2;

		}

		PlaneGeometry::~PlaneGeometry()
		{
		}
		std::shared_ptr<PolygonPleTextureBinder> PlaneGeometry::CreateUVBinder()
		{
			return std::make_shared<PolygonPleTextureBinder>(4);
		}
	}
}


