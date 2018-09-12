#pragma once
#include "Geometry.hpp"
namespace XGF
{
	namespace Shape
	{
		class PlaneGeometry : public Geometry
		{
		public:
			PlaneGeometry(float width, float height);
			~PlaneGeometry();
			virtual std::shared_ptr<PolygonPleTextureBinder> CreateUVBinder();
		};
	}
}

