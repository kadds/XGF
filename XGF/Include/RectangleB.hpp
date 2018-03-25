#pragma once
#include "Shape2D.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	namespace Shape
	{
		//双矩形，用于9path
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
			virtual bool IsInBoundBox(const Point & p) override;
			//always return 2
			int GetBorderLayer(int layer[]);
			//获取内矩形
			void GetInerBorderRectangle(Rectangle & rc);
		};
	}

}