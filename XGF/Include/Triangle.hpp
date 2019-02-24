#pragma once
#include "Defines.hpp"
#include "Shape2D.hpp"
namespace XGF
{
	namespace Shape
	{
		class Triangle :
			public Shape2D
		{
		public:
			Triangle();
			~Triangle();
			void SetThreePoint(const Point & p1, const Point & p2, const Point & p3);
			virtual void SetPosition(const Point & p);
			virtual void GetPosition(Point & p) const override;
			virtual bool IsInBoundBox(const Point & p, const SM::Matrix & matrix) override;
		};
	}
}


