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
			//precision：精度(三角形个数)
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
//获取精度
//参数一：半径大小
//参数二：三角形个数
#define GetPrecision(radius,arclen) DirectX::XM_PI/arclen * radius 

