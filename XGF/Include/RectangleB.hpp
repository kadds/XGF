#pragma once
#include "Shape.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	namespace Shape
	{
		//˫���Σ�����9path
		class RectangleB :
			public Shape
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
			//��ȡ�ھ���
			void GetInerBorderRectangle(Rectangle & rc);
		};
	}

}