#pragma once
#include "Shape2D.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	namespace Shape
	{
		//带边框的矩形
		class RectangleC :
			public Shape2D
		{
		public:
			RectangleC();
			virtual ~RectangleC();
			void SetPositionAndSize(float x, float y, float width, float height);
			void SetBorderSize(float width);
			virtual void SetPosition(const Point & p) override;
			virtual void GetPosition(Point & p) const override;
			virtual bool IsInBoundBox(const Point & p) override;
			//always return 3
			int GetBorderLayer(int layer[]);
			//获取内矩形
			void GetInerBorderRectangle(Rectangle & rc);
		};
	}

}