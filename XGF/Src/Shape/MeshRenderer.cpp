#include "..\..\Include\MeshRenderer.hpp"
#include "../../Include/Mesh.hpp"
#include "../../Include/Material/LightMaterial.hpp"
#include "../../Include/Light/Light.hpp"
#include "../../Include/Renderer.hpp"
#include "../../Include/Camera.hpp"

namespace XGF
{
	using XGF::Batch;
	using XGF::BindingBridge;
	namespace Shape
	{
		RenderResource& MeshRenderResourceCachePool::GetRenderResource(Shaders shaders)
		{
			auto it = mCachedRenderResource.find(shaders);
			if (it != mCachedRenderResource.end())
			{
				return it->second;
			}
			else
			{
				return mCachedRenderResource.insert_or_assign(shaders, RenderResource(shaders)).first->second;
			}
		}

		void MeshRenderResourceCachePool::ClearCachedRenderResource()
		{
			mCachedRenderResource.clear();
		}

		void ForwardRenderPath::Draw(MeshRenderResourceCachePool & cachePool, Camera& camera, std::vector<Mesh*>& meshs, std::vector<Light*>& lights)
		{
			for (auto light : lights)
			{
				if(light->GetLightType() != LightType::Ambient)
				{
					((CastShadowAbleLight *)light)->GenerateShadowMap(meshs);
				}
			}
			for (auto mesh : meshs)
			{
				mesh->GetMeshRenderPath()->Draw(cachePool, camera, mesh, lights);
			}
		}
	}
}

