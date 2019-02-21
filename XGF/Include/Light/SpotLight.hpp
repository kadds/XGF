#pragma once
#include "Light.hpp"
#include "../Camera.hpp"

namespace XGF::Shape
{
	class SpotLight : public CastShadowAbleLight
	{
	public:
		SpotLight(const Point& position, const Point& direction, const Color& lightColor,
		          float cutoff = DirectX::XM_PIDIV4 * 4 / 3, float cutoffOuter = DirectX::XM_PIDIV2,
		          float constant = 1.f, float linear = 0.1f, float quadratic = 0.01f);

		SpotLight(const Point& position, const Point& direction, const Color& lightColor, const Point& atten,
		          float cutoff = DirectX::XM_PIDIV4 * 4 / 3, float cutoffOuter = DirectX::XM_PIDIV2);

		void SetAttenuation(const Point& atte);

		void SetAttenuationConstant(float constant);

		void SetAttenuationLinear(float linear);

		void SetAttenuationQuadratic(float quadratic);

		float GetAttenuationQuadratic() const;

		float GetAttenuationLinear() const;

		float GetAttenuationConstant() const;

		Point GetAttenuation() const;

		virtual int CopyConstantBuffer(void* dest) override;

		virtual LightShadowMatrix GetLightMatrix() override;

		void SetShadowArea(float fov, float xnear, float xfar);

	private:
		float mCutoffOuter;
		float mCutoff;
		Point mAttenuation;
		float mNear;
		float mFar;
		float mFov;
	};
}
