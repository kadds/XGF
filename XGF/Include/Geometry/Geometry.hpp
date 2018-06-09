#pragma once
#include "../Shape.hpp"
namespace XGF 
{
	namespace Shape 
	{
		class Geometry : public Shape
		{
		public:
			Geometry(unsigned int vertexCount, unsigned int indexCount);
			~Geometry();
		};
	}
}
