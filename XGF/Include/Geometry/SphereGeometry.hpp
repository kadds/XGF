#pragma once
#include "Geometry.hpp"
#include "../Polygon.hpp"

namespace XGF
{
	namespace Shape
	{
		class SphereGeometryMapping
		{
		public:
			SphereGeometryMapping(Geometry *) {  }
			Point2 operator()(float x, float y)
			{
				return Point2(x, y);
			};
		};

		class SphereGeometry : public Geometry
		{
		public:
			template<typename GeometryMapping = SphereGeometryMapping>
			SphereGeometry(float radius, int widthSegments, int heightSegments, float phiStart = 0, float phiLength = DirectX::XM_2PI, float thetaStart = 0, float thetaLength = DirectX::XM_PI)
			: Geometry((heightSegments + 1) * (widthSegments + 1), heightSegments * widthSegments * 6),
			mWidthSegments(widthSegments), mHeightSegments(heightSegments),
			mRadius(radius), mPhiStart(phiStart), mPhiLength(phiLength), mThetaStart(thetaStart), mThetaLength(thetaLength)
			{
				Init();
				GenerateMapping();
			}

			~SphereGeometry() = default;
		private:
			template<typename GeometryMapping = SphereGeometryMapping>
			void GenerateMapping()
			{
				mPolygonPleUvs = std::make_shared<PolygonPleTextureBinder>(mPolygon->Count());
				GeometryMapping mapping(this);

				for (int i = 0; i <= mHeightSegments; i++)
				{
					float h = i / (float) (mHeightSegments + 1);
					for (int j = 0; j <= mWidthSegments; j++)
					{
						float w = j / (float)(mWidthSegments + 1);
						mPolygonPleUvs->GetData(i * (mWidthSegments + 1) + j) = mapping(w, h);
					}
				}

			};

			void Init();
		private:
			int mWidthSegments, mHeightSegments;
			float mRadius;
			float mPhiStart, mPhiLength, mThetaStart, mThetaLength;
		};
	}
}