#include "../../../Include/Light/Light.hpp"
namespace XGF::Shape
{
	Light::Light(const Point& position, const Color& lightColor, LightType lightType):
		mLightColor(lightColor), mLightType(lightType), mPosition(position)
	{
		mTransform.SetTranslation(position);
	}

	Transform& Light::GetTransform()
	{
		return mTransform;
	}

	void Light::SetGroup(int group)
	{
		mGroup = group;
	}

	int Light::GetGroup() const
	{
		return mGroup;
	}

	
	LightType Light::GetLightType() const
	{
		return mLightType;
	}

	Color Light::GetLightColor() const
	{
		return mLightColor;
	}
}
