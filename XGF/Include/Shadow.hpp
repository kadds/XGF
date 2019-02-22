#pragma once
#include "Defines.hpp"

namespace XGF::Shape
{
	class CastShadowAble
	{
		ClassPropertyWithInitAndChanged(CastShadow, bool, false, OnCastShadowPropertyChanged)
		ClassPropertyWithInit(SlopeScaledDepthBias, float, 0.f)
		ClassPropertyWithInit(DepthBias, int, 0)
		ClassPropertyWithInit(DepthBiasClamp, float, 0.0f)
	protected:
		virtual void OnCastShadowPropertyChanged() = 0;
	};
	enum class ShadowType
	{
		Default = 0u,
		PCF,
		SoftPCCS,
	};
	class ReceiveShadowAble
	{
		ClassPropertyWithInitAndChanged(ReceiveShadow, bool, false, OnReceiveShadowPropertyChanged)
		ClassPropertyWithInit(ShadowType, ShadowType, ShadowType::Default)
	protected:
		virtual void OnReceiveShadowPropertyChanged() = 0;
	};

}