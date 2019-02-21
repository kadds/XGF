#include  "./../../../Include/Light/DirectionalLight.hpp"

namespace XGF::Shape
{
	DirectionalLight::DirectionalLight(const Point& direction, const Color& lightColor): CastShadowAbleLight(Point::Zero, lightColor,
	                                                                                           LightType::Directional)
	{
		mTransform.SetRotationAngle(direction);
	}

	int DirectionalLight::CopyConstantBuffer(void* dest)
	{
		Color* destc = (Color *)dest;
		*destc++ = GetLightColor();
		*((Point*)destc) = GetTransform().GetRotation() * Light::DefaultDirection;
		return sizeof(Color) * 2;
	}

	LightShadowMatrix DirectionalLight::GetLightMatrix()
	{
		LightShadowMatrix wvpx;
		auto proj = DirectX::XMMatrixOrthographicOffCenterLH(mShadowArea.left, mShadowArea.right, mShadowArea.bottom, mShadowArea.top, mShadowArea.nearZ, mShadowArea.farZ);
		auto view = DirectX::XMMatrixLookToLH(mTransform.GetTranslate(), mTransform.GetRotation() * Light::DefaultDirection, Point(0, 1, 0));
		wvpx.viewProj = view * proj;
		return wvpx;
	}

	void DirectionalLight::SetShadowArea(const Point & from, const CameraRectangle& rec)
	{
		mTransform.SetTranslation(from);
		mShadowArea = rec;
	}
}
