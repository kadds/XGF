#pragma once
#include "../Defines.hpp"
#include "Light.hpp"

namespace XGF::Shape
{
	class PointLight : public CastShadowAbleLight
	{
	public:
		PointLight(const Point& position, const Color& lightColor, float constant = 1.f, float linear = 0.1f,
		           float quadratic = 0.01f);

		PointLight(const Point& position, const Color& lightColor, const Point& attenuation);

		void SetAttenuation(const Point& atte);

		void SetAttenuationConstant(float constant);

		void SetAttenuationLinear(float linear);

		void SetAttenuationQuadratic(float quadratic);

		float GetAttenuationQuadratic() const;

		float GetAttenuationLinear() const;

		float GetAttenuationConstant() const;

		Point GetAttenuation() const;

		virtual int CopyConstantBuffer(void* dest) override;
	private:
		Point mAttenuation;
	};
}
