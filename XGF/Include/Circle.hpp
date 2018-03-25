#pragma once
#include "Shape2D.hpp"
namespace XGF
{
	namespace Shape
	{
		class Circle :
			public Shape2D
		{
		public:
			//precision������(�����θ���)
			Circle(int precision);
			~Circle();
			void SetPosiotionAndRadius(float x, float y, float r);
			virtual bool IsInBoundBox(const Point & p) override;

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
//���������⾶��
#define GetPrecision(radius,arclen) (static_cast<float>(DirectX::XM_PI/arclen * radius))

