#include "../../../Include/Light/Light.hpp"
#include "../../../Include/Context.hpp"
#include "../../../Include/Renderer.hpp"
#include "../../../Include/Logger.hpp"

namespace XGF::Shape
{
	Light::Light(const Point& position, const Color& lightColor, LightType lightType):
		mLightType(lightType), mLightColor(lightColor)
	{
		mTransform.SetTranslation(position);
	}

	Light::~Light()
	{
	}

	Transform& Light::GetTransform()
	{
		return mTransform;
	}

	LightType Light::GetLightType() const
	{
		return mLightType;
	}

	Texture* CastShadowAbleLight::GetShadowMapTexture() const
	{
		if(mShadowGenerator)
		return mShadowGenerator->GetTexture();
		else return nullptr;
	}

	void CastShadowAbleLight::OnCastShadowPropertyChanged()
	{
		ReCreateRenderResource();
	}

	LightShadowMatrix CastShadowAbleLight::GetLightMatrix()
	{
		XGF_Error(Framework, "This light type can not cast shadow.");
	}

	CastShadowAbleLight::~CastShadowAbleLight()
	{
		if(GetCastShadow() && mShadowGenerator)
		{
			mShadowGenerator->Shutdown();
			mShadowGenerator.reset();
		}
	}

	void CastShadowAbleLight::SetShadowMapSize(int width, int height)
	{
		mWidth = width;
		mHeight = height;
	}

	void CastShadowAbleLight::GenerateShadowMap(const std::vector<Mesh *> & meshes)
	{
		if (!mShadowGenerator) return;
		LightShadowMatrix wvpx = GetLightMatrix();
		mShadowGenerator->Generate(this, meshes, wvpx);
	}

	void CastShadowAbleLight::ReCreateRenderResource()
	{
		auto & renderer = Context::Current().QueryRenderer();
		if (GetCastShadow())
		{
			if (mShadowGenerator)
			{
				renderer.AppendAfterDrawCallbackOnce([this](Renderer * r)
				{
					mShadowGenerator->Shutdown();
					r->AppendBeforeDrawCallbackOnce([this](Renderer *)
					{
						mShadowGenerator->Initialize(mWidth, mHeight);
					});
				});
			}
			else
			{
				renderer.AppendBeforeDrawCallbackOnce([this](Renderer *)
				{
					mShadowGenerator = std::make_unique<ShadowMapGenerator>();
					mShadowGenerator->Initialize(mWidth, mHeight);
				});
			}
		}
		else
		{
			if(mShadowGenerator)
			{
				renderer.AppendAfterDrawCallbackOnce([this](Renderer * r)
				{
					mShadowGenerator->Shutdown();
					mShadowGenerator.reset();
				});
			}
		}
	}

	void Light::SetDirection(const Point& direction)
	{
		mTransform.SetRotationAngle(direction);
	}

	Point Light::GetDirection() const
	{
		return mTransform.GetRotation();
	}
}
