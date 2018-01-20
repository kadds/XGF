#pragma once
#include "Shape.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	namespace Shape
	{
		//带边框的矩形
		class RectangleC :
			public Shape
		{
		public:
			RectangleC();
			virtual ~RectangleC();
			void SetPositionAndSize(float x, float y, float width, float height);
			void SetBorderSize(float width);
			virtual void SetPosition(const Point & p) override;
			virtual void GetPosition(Point & p) const override;
			virtual bool IsInBoundBox(const Point & p, const FXMMATRIX matrix) const override;
			//always return 3
			int GetBorderLayer(int layer[]);
			//获取内矩形
			void GetInerBorderRectangle(Rectangle & rc);
		};
	}

}