#pragma once
#include "Geometry.hpp"
#include "../Polygon.hpp"

namespace XGF::Shape
{
	class PlaneGeometryMapping
	{
	public:
		PlaneGeometryMapping(Geometry *) {  }
		Point2 operator()(int index, float x, float y)
		{
			return Point2(x, y);
		};
	};

	class PlaneGeometry : public Geometry
	{
	public:
		PlaneGeometry(float width, float height, int widthSegments = 1, int heightSegments = 1)
			: Geometry((widthSegments + 1) * (heightSegments + 1), widthSegments * heightSegments * 6), mWidth(width), mHeight(height),
			mWidthSegments(widthSegments), mHeightSegments(heightSegments)
		{
			Init();
			GenerateMapping<>();
		}
		template<typename GeometryMapping = PlaneGeometryMapping>
		void GenerateMapping()
		{
			mPolygonPleUvs = std::make_shared<PolygonPleTextureBinder>(mPolygon->Count());
			GeometryMapping mapping(this);
			int count = mPolygon->Count();
			for (int j = 0; j < count; j++)
			{
				mPolygonPleUvs->GetData(j) = mapping(j, mPolygon->GetData(j).x / mWidth + 0.5f, mPolygon->GetData(j).z / mHeight + 0.5f);
			}

		};
		
		virtual ~PlaneGeometry() = default;
	protected:
		void Init();
		int mWidthSegments;
		int mHeightSegments;
		float mWidth, mHeight;
	};

}

