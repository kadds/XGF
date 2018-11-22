#pragma once
#include "../Shape.hpp"
#include <vector>
#include <functional>
namespace XGF
{
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
			std::vector<std::shared_ptr<PolygonPleBinder>> GetMapBinder();
		public:
			std::shared_ptr<PolygonPlePoint3> mPolygonPleNormal;
			std::shared_ptr<PolygonPleTextureBinder> mPolygonPleUvs;
			std::shared_ptr<PolygonPleTextureBinder> mPolygonPleColor;
		protected:
			virtual std::vector< std::shared_ptr<PolygonPleBinder>> GetExtraBinders() { return std::vector< std::shared_ptr<PolygonPleBinder>>(); };
		};
	}
}
