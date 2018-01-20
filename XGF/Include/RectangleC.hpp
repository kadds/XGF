#pragma once
#include "Shape.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	namespace Shape
	{
		//���߿�ľ���
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
			//��ȡ�ھ���
			void GetInerBorderRectangle(Rectangle & rc);
		};
	}

}