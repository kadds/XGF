#pragma once
#include "../Defines.hpp"
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
			SphereGeometry(float radius, int widthSegments, int heightSegments, float phiStart = 0, float phiLength = DirectX::XM_2PI, float thetaStart = 0, float thetaLength = DirectX::XM_PI)
			: Geometry((heightSegments + 1) * (widthSegments + 1), heightSegments * widthSegments * 6),
			mWidthSegments(widthSegments), mHeightSegments(heightSegments),
			mRadius(radius), mPhiStart(phiStart), mPhiLength(phiLength), mThetaStart(thetaStart), mThetaLength(thetaLength)
			{
				Init();
				GenerateMapping<>();
			}
			template<typename GeometryMapping = SphereGeometryMapping>
			void GenerateMapping()
			{
				mPolygonPleUvs = std::make_shared<PolygonPleTextureBinder>(mPolygon->GetActualCount());
				GeometryMapping mapping(this);

				for (int i = 0; i <= mHeightSegments; i++)
				{
					float h = i / (float)(mHeightSegments + 1);
					for (int j = 0; j <= mWidthSegments; j++)
					{
						float w = j / (float)(mWidthSegments + 1);
						mPolygonPleUvs->GetData(i * (mWidthSegments + 1) + j) = mapping(w, h);
					}
				}

			};
			~SphereGeometry() = default;
		protected:
			void Init();
			int mWidthSegments, mHeightSegments;
			float mRadius;
			float mPhiStart, mPhiLength, mThetaStart, mThetaLength;
		};
	}
}