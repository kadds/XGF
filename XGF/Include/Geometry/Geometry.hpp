#pragma once
#include "../Shape.hpp"
#include <vector>
namespace XGF 
{
	namespace Shape 
	{
		class Geometry : public Shape
		{
		public:
			Geometry(unsigned int vertexCount, unsigned int indexCount);
			~Geometry();
			virtual std::shared_ptr<PolygonPleTextureBinder> CreateUVBinder() = 0;
		private:
			std::vector<int> mFace;
		};
	}
}
