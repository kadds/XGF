#pragma once
#include "../Defines.hpp"
#include "../Transform.hpp"

namespace XGF::Shape
{
	enum class LightType
	{
		Ambient = 0u,
		Directional,
		Point,
		Spot,

		InvalidValue
	};
	class Light
	{
	public:
		Light(const Point& position, const Color& lightColor, LightType lightType);

		Transform& GetTransform();

		void SetGroup(int group);

		int GetGroup() const;

		virtual int CopyConstantBuffer(void * dest) = 0;
		LightType GetLightType() const;

		Color GetLightColor() const;
	protected:
		
	private:
		LightType mLightType;
		Transform mTransform;
		Color mLightColor;
		int mGroup;
		const Point& mPosition;
	};
}
