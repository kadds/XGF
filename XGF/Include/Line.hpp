#pragma once
#include "Defines.hpp"
#include "Shape.hpp"
namespace XGF
{
	namespace Shape 
	{
		class Line :
			public Shape
		{
		public:
			Line();
			virtual ~Line();
			virtual void SetPosition(const Point & p) override;
			void SetEndPosition(Point p);
			virtual void GetPosition(Point & p) const override;
		private:
		};
	}
	

}

