#pragma once
#include "Defines.hpp"
#include "Shape.hpp"
namespace XGF 
{
	namespace Shape
	{
		class Shape2D : public Shape
		{
		public:
			Shape2D(int n, int indexCount);
			~Shape2D();
			virtual bool IsInBoundBox(const Point & p, const SM::Matrix & matrix) { return false; };

		};
	}
	
}
