#pragma once
#include "Shape.hpp"
namespace XGF
{
	namespace Shape
	{
		class Circle :
			public Shape
		{
		public:
			//precision������(�����θ���)
			Circle(int precision);
			~Circle();
			void SetPosiotionAndRadius(float x, float y, float r);
			virtual bool IsInBoundBox(const Point & p, const FXMMATRIX matrix) const override;
			virtual void SetPosition(const Point & p);
			//always return 2
			int GetLayer(int layer[]);
		private:
			float mRadius;
		};
	}
	
}
//��ȡ����
//����һ���뾶��С
//�������������θ���
#define GetPrecision(radius,arclen) DirectX::XM_PI/arclen * radius 

