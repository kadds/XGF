#include "../../Include/Rectangle.hpp"
#include "../../Include/Control.hpp"
namespace XGF
{
	namespace Shape
	{
		Rectangle::Rectangle() :Shape2D(4, 6)
		{
			mPolygonPleIndex->mIndex[0] = 0;
			mPolygonPleIndex->mIndex[1] = 3;
			mPolygonPleIndex->mIndex[2] = 2;
			mPolygonPleIndex->mIndex[3] = 2;
			mPolygonPleIndex->mIndex[4] = 1;
			mPolygonPleIndex->mIndex[5] = 0;
		}


		Rectangle::~Rectangle()
		{

		}

		void Rectangle::SetPositionAndSize(float x, float y, float width, float height)
		{
			auto shapePos = mPolygon->GetData();
			shapePos[0].x = shapePos[1].x = x;
			shapePos[0].y = shapePos[3].y = y;
			shapePos[2].x = shapePos[3].x = x + width;
			shapePos[1].y = shapePos[2].y = y + height;
		}


		bool Rectangle::IsInBoundBox(const Point & p, const SM::Matrix & matrix)
		{
			auto ple = std::make_shared<PolygonPlePointBinder>(4);
			mPolygon->ExpandAllTo(*ple.get(), Operator::Multiply(matrix));
			return pInPolygon(ple, p.x, p.y);
		}

		bool Rectangle::SubRectangle(Rectangle * pol, float x, float y, float width, float height) const
		{
			auto to = pol->mPolygon->GetData(), from = mPolygon->GetData();
			to[0].x = to[1].x = from[0].x + x;
			to[2].x = to[3].x = from[0].x + x + width;

			to[0].y = to[3].y = from[0].y + y;
			to[2].y = to[1].y = from[0].y + y + height;
			to[3].z = from[3].z;
			to[0].z = from[0].z;
			to[2].z = from[2].z;
			to[1].z = from[1].z;
			return to[2].y <= from[2].y && to[2].x <= from[2].x;
		}
		void Rectangle::SetPosition(const Point & p)
		{
			auto shapePos = mPolygon->GetData();
			shapePos[2].x = shapePos[3].x += p.x - shapePos[0].x;
			shapePos[1].y = shapePos[2].y += p.y - shapePos[0].y;
			shapePos[0].x = shapePos[1].x = p.x;
			shapePos[0].y = shapePos[3].y = p.y;
			shapePos[0].z = shapePos[1].z = shapePos[2].z = shapePos[3].z = p.z;
		}

		void Rectangle::GetPosition(Point & p) const
		{
			p = mPolygon->GetData(0);
		}

	}
	
}
