#pragma once
#include "Geometry.hpp"
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
				mPolygonPleUvs = std::make_shared<PolygonPleTextureBinder>(mPolygon->mCount);
				GeometryMapping mapping(this);

				for (int i = 0; i <= mHeightSegments; i++)
				{
					float h = i / (float) (mHeightSegments + 1);
					for (int j = 0; j <= mWidthSegments; j++)
					{
						float w = j / (float)(mWidthSegments + 1);
						mPolygonPleUvs->mPoint[i * (mWidthSegments + 1) + j] = mapping(w, h);
					}
				}

			};

			void Init()
			{
				mPolygonPleNormal = std::make_shared<PolygonPlePoint3>(mPolygon->mCount);
				auto * normal = mPolygonPleNormal->mPoint;
				const float dw = 1.f / mWidthSegments;
				const float dh = 1.f / mHeightSegments;
				const float phiStart = mPhiStart;
				const float phiLength = mPhiLength;
				const float thetaStart = mThetaStart;
				const float thetaLength = mThetaLength;
				const float radius = mRadius;
				const int widthSegments = mWidthSegments;
				const int heightSegments = mHeightSegments;
				Index * in = mPolygonPleIndex->mIndex;
				const auto aw = widthSegments + 1;
				for (int i = 0; i <= heightSegments; i++)
				{
					const auto v = dh * i;
					const auto row = i * aw;
					for (int j = 0; j <= widthSegments; j++)
					{
						const auto u = dw * j;
						
						auto & p = mPolygon->mPoint[row + j];
						p.x = - radius * sin(thetaStart + v * thetaLength) * cos(phiStart + u * phiLength);
						p.y = radius * cos(thetaStart + v * thetaLength);
						p.z = radius * sin(phiStart + u * phiLength) * sin(thetaStart + v * thetaLength);
						normal->Set(p.x, p.y, p.z);
						normal->Normalize();
						normal++;
					}
				}
				const auto thetaEnd = thetaStart + thetaLength;
				
				int cIndex = 0;
				for (int i = 0; i < heightSegments; i++)
				{
					for (int j = 0; j < widthSegments; j++)
					{
						Index a = i * aw + j + 1;
						Index b = i * aw + j;
						Index c = (i + 1) * aw + j;
						Index d = (i + 1) * aw + j + 1;
						if (i != 0 || thetaStart > 0)
						{
							*(in++) = a;
							*(in++) = b;
							*(in++) = d;
							cIndex += 3;
						}
						if (i != heightSegments - 1 || thetaEnd < DirectX::XM_PI)
						{
							*(in++) = b;
							*(in++) = c;
							*(in++) = d;
							cIndex += 3;
						}
					}
				}
				mPolygonPleIndex->ShrinkTo(cIndex);
			}
		private:
			int mWidthSegments, mHeightSegments;
			float mRadius;
			float mPhiStart, mPhiLength, mThetaStart, mThetaLength;
		};
	}
}