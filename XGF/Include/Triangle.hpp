#pragma once
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
			void SetThreePoint(Point &p1, Point &p2, Point &p3);
			virtual void SetPosition(const Point & p);
			virtual void GetPosition(Point & p) const override;
			virtual bool IsInBoundBox(const Point & p) override;
		};
	}
}


