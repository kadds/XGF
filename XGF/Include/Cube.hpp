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
			virtual bool IsInBBox(const Point & p, const XMFLOAT4X4 * matrix) const override { return false; };
			virtual void SetPosition(const Point & p) override;
			void Filp();
		};
	}
	
}

