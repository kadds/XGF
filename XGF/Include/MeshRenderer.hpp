#pragma once
#include "Defines.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include <algorithm>
#include "Light/Light.hpp"

namespace XGF
{
	class Camera;
	namespace Shape 
	{
		class Light;

		class Mesh;
		class MeshRenderResourceCachePool
		{
		private:
			std::unordered_map<Shaders, RenderResource, ShadersHash> mCachedRenderResource;
		public:
			RenderResource& GetRenderResource(Shaders shaders);

			void ClearCachedRenderResource();
		};
		template<typename RenderPath>
		class MeshRenderer
		{
		public:
			void Initialize()
			{
				
			}
			void Shutdown()
			{
				mRenderMeshes.clear();
				mLights.clear();
			}
			MeshRenderer() = default;
			~MeshRenderer() = default;
			void Add(Mesh * mesh)
			{
				if (auto ele = std::find(mRenderMeshes.begin(), mRenderMeshes.end(), mesh); ele == mRenderMeshes.end())
				{
					mRenderMeshes.push_back(mesh);
				}
				auto rp = RenderPath::NewRenderPath(mesh);
				mesh->SetMeshRenderPath(std::move(rp));
			}
			void Remove(Mesh * mesh)
			{
				if (auto ele = std::find(mRenderMeshes.begin(), mRenderMeshes.end(), mesh); ele != mRenderMeshes.end())
				{
					(*ele)->GetMeshRenderPath()->Clear(*ele, mLights);
					mRenderMeshes.erase(ele);
				}
			}
			void Add(Light * light)
			{
				mLights.push_back(light);
			}
			void Remove(Light * light)
			{
				auto it = std::find(mLights.begin(), mLights.end(), light);
				if (it != mLights.end())
				{
					mLights.erase(it);
				}
			}

			void Begin(Camera & camera)
			{
				mCamera = &camera;
				RenderPath::Begin(camera);
			}
			
			void Draw()
			{
				RenderPath::Draw(mCachePool, *mCamera, mRenderMeshes, mLights);
			}
			void End()
			{
				RenderPath::End();
			}
			void ReBuild()
			{
				std::stable_sort(mLights.begin(), mLights.end(), [](const Light * light1, const Light * light2)
				{
					return (int)light1->GetLightType() < (int)light2->GetLightType();
				});
				for (auto renderMesh : mRenderMeshes)
				{
					renderMesh->GetMeshRenderPath()->ReCreate(renderMesh, mLights);
				}
			}
		private:
			std::vector<Mesh *> mRenderMeshes;
			std::vector<Light *> mLights;
			Camera * mCamera;
			MeshRenderResourceCachePool mCachePool;
		};

		class ForwardRenderPath
		{
		public:
			static void Begin(Camera & camera)
			{

			}

			static void Draw(MeshRenderResourceCachePool & cachePool, Camera& camera, std::vector<Mesh *>& meshs, std::vector<Light *>& lights);

			static void End()
			{

			}
			static std::unique_ptr<MeshRenderPath> NewRenderPath(Mesh * mesh)
			{
				return std::make_unique<MeshForwardRenderPath>(mesh);
			}

		};


	}
}
