#pragma once
#include "Light.hpp"

namespace XGF::Shape
{
	class DirectionalLight : public Light
	{
	public:
		DirectionalLight(const Point& direction, const Color& lightColor);

		virtual int CopyConstantBuffer(void* dest) override;

		void SetDirection(const Point& direction);

		Point GetDirection() const;
	private:
		Point mLightDirection;
	};
}
