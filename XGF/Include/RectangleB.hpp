#pragma once
#include "Shape.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	namespace Shape
	{
		//���߿�ľ���
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
			virtual bool IsInBBox(const Point & p, const XMFLOAT4X4 * matrix) const override;
			//always return 3
			int GetBorderLayer(int layer[]);
			//��ȡ�ھ���
			void GetInerBorderRectangle(Rectangle & rc);
		};
	}

}