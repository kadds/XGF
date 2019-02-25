#include "..\..\..\Include\Geometry\Geometry.hpp"
#include "../../../Include/Polygon.hpp"

namespace XGF
{
	namespace Shape
	{
		Geometry::Geometry(unsigned int vertexCount, unsigned int indexCount) :Shape(vertexCount, indexCount)
		{
		}

		Geometry::~Geometry()
		{
		}

		std::shared_ptr<PolygonPleBinder> Geometry::GetVertexBinder()
		{
			return mPolygon;
		}

		std::shared_ptr<PolygonPleBinder> Geometry::GetUVBinder()
		{
			return mPolygonPleUvs;
		}
		std::shared_ptr<PolygonPleBinder> Geometry::GetNormalBinder()
		{
			return mPolygonPleNormal;
		}
		std::vector<std::shared_ptr<PolygonPleBinder>> Geometry::GetDataBinders()
		{
			std::vector<std::shared_ptr<PolygonPleBinder>> v(0);
			return v;
		}
	}
}


