#pragma once
#include "Defines.hpp"
#include "Shape2D.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	namespace Shape
	{
		class RectangleB :
			public Shape2D
		{
		public:
			RectangleB();
			virtual ~RectangleB();
			void SetPositionAndSize(float x, float y, float width, float height);
			void SetBorderSize(float width);
			virtual void SetPosition(const Point & p) override;
			virtual void GetPosition(Point & p) const override;
			virtual bool IsInBoundBox(const Point & p, const SM::Matrix & matrix) override;
			//always return 2
			int GetBorderLayer(int layer[]);
			void GetInerBorderRectangle(Rectangle & rc);
		};
	}

}