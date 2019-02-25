#pragma once
#include "../Defines.hpp"
#include "../Transform.hpp"
#include "../ShadowGenerator.hpp"

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
		virtual ~Light();
		Transform& GetTransform();
		LightType GetLightType() const;

		void SetDirection(const Point& direction);
		const Point& GetDirection() const;
		Point& GetDirection();
	protected:
		inline static const Point DefaultDirection = Point(1, 1, 1);
		Transform mTransform;
	private:
		LightType mLightType;
		ClassProperty(Group, int)
		ClassProperty(LightColor, Color)
		DisableCopyAndAssign(Light)
	};

	class CastShadowAbleLight : public Light, public CastShadowAble
	{
		ClassPropertyWithInit(ShadowType, ShadowType, ShadowType::Default)
	protected:
		int mWidth, mHeight;
	public:
		using Light::Light;
		~CastShadowAbleLight();
		void SetShadowMapSize(int width, int height);

		void GenerateShadowMap(const std::vector<Mesh *> & meshes);

		void ReCreateRenderResource();
		virtual LightShadowMatrix GetLightMatrix();
		virtual int CopyConstantBuffer(void * dest) = 0;
		Texture* GetShadowMapTexture() const;
		void OnCastShadowPropertyChanged() override;
	private:
		
		std::unique_ptr<ShadowMapGenerator> mShadowGenerator;
		DisableCopyAndAssign(CastShadowAbleLight)
	};
}
