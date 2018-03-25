#pragma once
#include "Shape.hpp"
namespace XGF
{
	namespace Shape
	{
		class Cube :
			public Shape
		{
		public:
			Cube();
			virtual ~Cube();
			void SetPositionAndSize(float x, float y, float z, float width, float height, float depth);
			void SetCenterPositionAndSize(XMFLOAT3 center, XMFLOAT3 & whd);

			virtual void SetPosition(const Point & p) override;
			void Filp();
		};
	}
	
}

