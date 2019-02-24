#pragma once
#include "Defines.hpp"
#include "Shape2D.hpp"
namespace XGF
{
	namespace Shape
	{
		class Circle :
			public Shape2D
		{
		public:
			//precision：count of triangle
			Circle(int precision);
			~Circle();
			void SetPositionAndRadius(float x, float y, float r);
			virtual bool IsInBoundBox(const Point & p, const SM::Matrix & matrix) override;

			virtual void SetPosition(const Point & p);
			//always return 2
			int GetLayer(int layer[]);
		private:
			float mRadius;
		};
	}
	
}

#define GetPrecision(radius,arclen) (static_cast<float>(DirectX::XM_PI/arclen * radius))

;
