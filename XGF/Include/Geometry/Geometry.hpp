#pragma once
#include "../Defines.hpp"
#include "../Shape.hpp"
#include <functional>
namespace XGF
{
	class PolygonPleBinder;
	template<typename T>
	class PolygonPleDataBinder;
	typedef PolygonPleDataBinder<Point> PolygonPleNormalBinder;
	typedef PolygonPleDataBinder<Point2> PolygonPleTextureBinder;

	namespace Shape
	{
		class Face
		{
			int inc[3];
		public:
			Face(int a, int b, int c) { inc[0] = a; inc[1] = b; inc[2] = c; }
		};
		class Face4
		{
			int inc[4];
		public:
			Face4(int a, int b, int c, int d) { inc[0] = a; inc[1] = b; inc[2] = c; inc[3] = d; }
		};

		class Geometry : public Shape
		{
		public:
			Geometry(unsigned int vertexCount, unsigned int indexCount);
			virtual ~Geometry();
			std::vector<std::shared_ptr<PolygonPleBinder>> GetVertexBinders();
			std::vector<std::shared_ptr<PolygonPleBinder>> GetUVBinders();
			std::vector<std::shared_ptr<PolygonPleBinder>> GetNormalBinders();
			std::vector<std::shared_ptr<PolygonPleBinder>> GetDataBinders();
		public:
			std::shared_ptr<PolygonPleNormalBinder> mPolygonPleNormal;
			std::shared_ptr<PolygonPleTextureBinder> mPolygonPleUvs;
			std::shared_ptr<PolygonPleTextureBinder> mPolygonPleColor;
		};
	}
}
