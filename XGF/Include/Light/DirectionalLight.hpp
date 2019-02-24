#pragma once
#include "../Defines.hpp"
#include "Light.hpp"
#include "../Camera.hpp"

namespace XGF::Shape
{
	class DirectionalLight : public CastShadowAbleLight
	{
	public:
		DirectionalLight(const Point& direction, const Color& lightColor);

		virtual int CopyConstantBuffer(void* dest) override;
		virtual  LightShadowMatrix GetLightMatrix() override;
		void SetShadowArea(const Point & from, const CameraRectangle& rec);

	private:
		CameraRectangle mShadowArea;

	};
}
