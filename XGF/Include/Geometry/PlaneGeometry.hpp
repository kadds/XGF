#pragma once
#include "Geometry.hpp"
#include <algorithm>

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
		template<typename GeometryMapping = PlaneGeometryMapping>
		PlaneGeometry(float width, float height, int widthSegments = 1, int heightSegments = 1)
			: Geometry((widthSegments + 1) * (heightSegments + 1), widthSegments * heightSegments * 6), mWidth(width), mHeight(height),
			mWidthSegments(widthSegments), mHeightSegments(heightSegments)
		{
			Init();
			GenerateMapping<GeometryMapping>();
		}

		void Init();
		virtual ~PlaneGeometry() {};
	private:
		template<typename GeometryMapping = PlaneGeometryMapping>
		void GenerateMapping()
		{
			mPolygonPleUvs = std::make_shared<PolygonPleTextureBinder>(mPolygon->mCount);
			GeometryMapping mapping(this);
			int count = mPolygon->mCount;
			for (int j = 0; j < count; j++)
			{
				mPolygonPleUvs->mPoint[j] = mapping(j, mPolygon->mPoint[j].x / mWidth + 0.5, mPolygon->mPoint[j].z / mHeight + 0.5);
			}

		};
	protected:
		int mWidthSegments;
		int mHeightSegments;
		float mWidth, mHeight;
	};

}

