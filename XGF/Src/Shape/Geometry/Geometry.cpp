#include "..\..\..\Include\Geometry\Geometry.hpp"

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

		std::vector<std::shared_ptr<PolygonPleBinder>> Geometry::GetMapBinder()
		{
			std::vector<std::shared_ptr<PolygonPleBinder>> binders;
			binders.push_back(mPolygon);
			if(mPolygonPleNormal)
				binders.push_back(mPolygonPleNormal);
			if (mPolygonPleUvs)
				binders.push_back(mPolygonPleUvs);
			if (mPolygonPleColor)
				binders.push_back(mPolygonPleColor);
			auto u = GetExtraBinders();
			binders.insert(binders.end(), u.begin(), u.end());
			return binders;
		}
	}
}


