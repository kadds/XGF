#pragma once
#include "Geometry.hpp"
#include "../Polygon.hpp"

namespace XGF
{
	namespace Shape
	{
		class BoxMapping
		{
		public:
			BoxMapping(Geometry *) {  }
			Point2 operator() (int face, float x, float y)
			{
				return Point2(x, y);
			}
		};


		class BoxGeometry : public Geometry
		{
		public:
			virtual ~BoxGeometry() {};
			BoxGeometry(float x, float y, float z, int xSegments = 1, int ySegments = 1, int zSegments = 1)
				: Geometry(24, 36), x(x), y(y), z(z), mXSegments(xSegments), mYSegments(ySegments), mZSegments(zSegments)
			{
				Init(x, y, z);
				GenerateMapping<>();
			}
			template<typename GeometryMapping = BoxMapping>
			void GenerateMapping()
			{
				mPolygonPleUvs = std::make_shared<PolygonPleTextureBinder>(mPolygon->Count());

				GeometryMapping mapping(this);
				auto tb = std::make_shared<PolygonPleTextureBinder>(6);
				int face = 6;
				auto data = mPolygonPleUvs->GetData();
				for (int j = 0; j < face; j++)
				{
					data[j * 4] = mapping(j, 0, 0);
					data[j * 4 + 1] = mapping(j, 1, 0);
					data[j * 4 + 2] = mapping(j, 1, 1);
					data[j * 4 + 3] = mapping(j, 0, 1);
				}

			};
		protected:
			void Init(float x, float y, float z);
			void InitializeIndex();
			float x, y, z;
			int mXSegments, mYSegments, mZSegments;
		};


	}
}
