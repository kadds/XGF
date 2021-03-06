#include "..\..\..\Include\Geometry\PlaneGeometry.hpp"
#include "../../../Include/Polygon.hpp"
namespace XGF::Shape
{
	void PlaneGeometry::Init()
	{
		mPolygon->SetFullActualCount();
		mPolygonPleNormal = std::make_shared<PolygonPleNormalBinder>(mPolygon->GetCapacity());
		mPolygonPleNormal->SetFullActualCount();
		float width = mWidth, height = mHeight;
		int widthSegments = mWidthSegments, heightSegments = mHeightSegments;
		XGF_ASSERT(widthSegments > 0 && heightSegments > 0);

		

		const auto halfWidth = width / 2;
		const auto halfHeight = height / 2;
		const auto segmentWidth = width / widthSegments;
		const auto segmentHeight = height / heightSegments;

		auto normal = Point(0, 1, 0);
		auto* vec = mPolygon->GetData();
		auto* inc = &mPolygonPleIndex->Get(0);
		mPolygonPleIndex->SetFullActualCount();

		auto* fc = mPolygonPleNormal->GetData();
		auto gs = heightSegments + 1;
		auto ms = widthSegments + 1;
		for (int i = 0; i < gs; i++)
		{
			float y = segmentHeight * i - halfHeight;
			for (int j = 0; j < ms; j++)
			{
				float x = segmentWidth * j - halfWidth;
				(vec++)->Set(x, 0, y);
				*(fc++) = normal;
			}
		}
		for (int i = 0; i < heightSegments; i++)
		{
			for (int j = 0; j < widthSegments; j++)
			{
				int a = j + ms * i;
				int b = j + ms * (i + 1);
				int c = (j + 1) + ms * (i + 1);
				int d = (j + 1) + ms * i;
				*(inc++) = a;
				*(inc++) = b;
				*(inc++) = d;
				*(inc++) = b;
				*(inc++) = c;
				*(inc++) = d;
			}
		}
	}
}


