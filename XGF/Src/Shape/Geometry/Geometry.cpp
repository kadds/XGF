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

		std::vector<std::shared_ptr<PolygonPleBinder>> Geometry::GetVertexBinders()
		{
			std::vector<std::shared_ptr<PolygonPleBinder>> v(1);
			v[0] = mPolygon;
			return v;
		}

		std::vector<std::shared_ptr<PolygonPleBinder>> Geometry::GetUVBinders()
		{
			std::vector<std::shared_ptr<PolygonPleBinder>> v(1);
			v[0] = mPolygonPleUvs;
			return v;
		}
		std::vector<std::shared_ptr<PolygonPleBinder>> Geometry::GetNormalBinders()
		{
			std::vector<std::shared_ptr<PolygonPleBinder>> v(1);
			v[0] = mPolygonPleNormal;
			return v;
		}
		std::vector<std::shared_ptr<PolygonPleBinder>> Geometry::GetDataBinders()
		{
			std::vector<std::shared_ptr<PolygonPleBinder>> v(0);
			return v;
		}
	}
}


