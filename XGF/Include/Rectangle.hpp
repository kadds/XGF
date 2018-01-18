#pragma once
#include "Shape.hpp"

#define SHAPE_RECTANGLE 4
namespace XGF 
{
	namespace Shape
	{
		class Rectangle : public Shape
		{
		public:
			Rectangle();
			virtual ~Rectangle();
			void SetPositionAndSize(float x, float y, float width, float height);

			virtual bool IsInBoundBox(const Point & p, const FXMMATRIX matrix) const override;
			bool SubRectangle(Rectangle * pol, float x, float y, float width, float height) const;
			virtual void SetPosition(const Point & p);
			virtual void GetPosition(Point & p) const override;
		private:

		};
	}
	
}

