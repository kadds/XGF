#pragma once
#include "Defines.hpp"
#include "Mesh.hpp"
#include "Light/Light.hpp"
#include "Camera.hpp"

namespace XGF
{
	class RenderPath
	{
	public:
		virtual void Render(Camera & camera, std::vector<Shape::Mesh *> & meshes, std::vector<Shape::Light *> & lights);
		virtual Shaders LoadShaders(Shape::Mesh * mesh);
	//{
	//		for (auto shaders : mesh->GetMaterial()->GetAllShaders())
	//		{
	//			if (auto re = std::find_if(mCachedRenderResources.begin(), mCachedRenderResources.end(), shaders); re == mCachedRenderResources.end())
	//			{
	//				mCachedRenderResources.emplace_back(shaders);
	//			}
	//		}
	//}
	protected:
		RenderState mRenderState;
		std::vector<RenderResource> mCachedRenderResources;
	};

	class ForwardRenderPath : public RenderPath
	{
		void Render(Camera& camera, std::vector<Shape::Mesh*>& meshes, std::vector<Shape::Light*>& lights) override
		{
			WVPMatrix wvp;
			camera.GetCameraMatrix(wvp);
			
			for (auto & light : lights)
			{
				if (light->GetLightType() != Shape::LightType::Ambient && dynamic_cast<Shape::CastShadowAbleLight *>(light)->GetCastShadow())
					dynamic_cast<Shape::CastShadowAbleLight *>(light)->GenerateShadowMap();
			}
		}
		Shaders LoadShaders(Shape::Mesh* mesh) override
		{
			auto material = mesh->GetMaterial();
			
		}
	};
}
