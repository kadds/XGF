#pragma once
#include "Defines.hpp"
#include "Geometry/Geometry.hpp"
#include "Material/Material.hpp"
#include "Shadow.hpp"

namespace XGF
{
	struct Shaders;
	class Camera;
	namespace Shape
	{
		class MeshRenderResourceCachePool;
		class Mesh;
		class Light;
		
		class MeshRenderPath
		{
		public:
			MeshRenderPath(Mesh * mesh){};
			virtual ~MeshRenderPath() {};

			virtual void Draw(MeshRenderResourceCachePool & cachePool, Camera & camera, Mesh * mesh, std::vector<Light*>& lights) = 0;
			virtual void Clear(Mesh* mesh, std::vector<Light*>& lights) = 0;
			virtual void ReCreate(Mesh* mesh, std::vector<Light*>& lights) = 0;
		protected:
			bool AddShaders(int key, Shaders shaders);

			Shaders FindShaders(int key);

			void ClearShaders();

		private:
			std::vector<std::pair<int, Shaders>> mShaders;

		};

		class MeshForwardRenderPath : public MeshRenderPath
		{
		private:
			void DrawBasicMaterial(MeshRenderResourceCachePool & cachePool, const WVPMatrix & wvp, Mesh * mesh);
			void DrawPhongOrLambertMaterial(MeshRenderResourceCachePool& cachePool, const WVPMatrix& wvp, Mesh* mesh,
			                       std::vector<Light*>& lights, Texture* textureMap,
			                       std::function<size_t(void* ptr, size_t size)> structData,
			                       std::function<size_t(void* ptr)> customData);

			void DrawPhongMaterial(MeshRenderResourceCachePool & cachePool, const WVPMatrix & wvp, Mesh * mesh, std::vector<Light*>& lights, Point viewPosition);
			void DrawLambertMaterial(MeshRenderResourceCachePool& cachePool, const WVPMatrix& wvp, Mesh* mesh, std::vector<Light*>& lights);
			void DrawPhysicsMaterial(MeshRenderResourceCachePool& cachePool, const WVPMatrix& wvp, Mesh* mesh, std::vector<Light*>& lights, Point viewPosition);
		public:
			void Draw(MeshRenderResourceCachePool& cachePool, Camera& camera, Mesh* mesh,
				std::vector<Light*>& lights) override;

			MeshForwardRenderPath(Mesh* mesh): MeshRenderPath(mesh)
			{

			}

			virtual void Clear(Mesh* mesh, std::vector<Light*>& lights)
			{
				
			}

			virtual void ReCreate(Mesh* mesh, std::vector<Light*>& lights);
		};

		class Mesh : public CastShadowAble
		{
		public:
			Mesh(std::unique_ptr<Geometry> geometry, std::unique_ptr<Material> material)
				: mGeometry(std::move(geometry)), mMaterial(std::move(material))
			{
			}
			~Mesh() = default;

			Material * GetMaterial() const
			{
				return mMaterial.get();
			}
			template<typename MaterialType>
			MaterialType * GetMaterialAs() const
			{
				return (MaterialType *)mMaterial.get();
			}
			std::unique_ptr<Geometry> LockGeometry()
			{
				return std::move(mGeometry);
			}
			void UnLockGeometry(std::unique_ptr<Geometry> m)
			{
				mGeometry = std::move(m);
			}
			Geometry * GetGeometry() const
			{
				return mGeometry.get();
			}
			MeshRenderPath * GetMeshRenderPath() const
			{
				return mRenderPath.get();
			}
		protected:
			void SetMeshRenderPath(std::unique_ptr<MeshRenderPath> renderPath)
			{
				mRenderPath = std::move(renderPath);
			}
			template<typename RenderPath> friend class MeshRenderer;
			std::unique_ptr<Geometry> mGeometry;
			std::unique_ptr<Material> mMaterial;
			std::unique_ptr<MeshRenderPath> mRenderPath;
			void OnCastShadowPropertyChanged() override
			{
				//mRenderPath->ReCreate(this);
			};
		};
	}
}

