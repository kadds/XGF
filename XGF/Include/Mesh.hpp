#pragma once
#include "Defines.hpp"
#include "Geometry/Geometry.hpp"
#include "Material/Material.hpp"

namespace XGF
{
	namespace Shape
	{
		class Mesh
		{
		public:
			Mesh(std::unique_ptr<Geometry> geometry, std::unique_ptr<Material> material)
				: mGeometry(std::move(geometry)), mMaterial(std::move(material))
			{
				if(!mMaterial->GetShaders().IsNullShaders())
					GenerateBindingBridge(mMaterial->GetShaders().vs->HasSemanticNormal(),
						mMaterial->GetShaders().vs->HasSemanticTexcoord(), false);
			}
			~Mesh() = default;

			std::unique_ptr<Material> LockMaterial()
			{
				return std::move(mMaterial);
			}

			BindingBridge & GetBindingBridge()
			{
				return mBindingBridge;
			}
			void UnLockMaterial(std::unique_ptr<Material> m)
			{
				mMaterial = std::move(m);
			}
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
			void GenerateBindingBridge(bool normal, bool uv, bool data)
			{
				if (mBindingBridge.Count() > 0)
				{
					mBindingBridge.Clear();
				}
				mBindingBridge.AddBinder(mGeometry->GetVertexBinders());
				if(normal)
					mBindingBridge.AddBinder(mGeometry->GetNormalBinders());
				if(uv)
					mBindingBridge.AddBinder(mGeometry->GetUVBinders());
				if(data)
					mBindingBridge.AddBinder(mGeometry->GetDataBinders());
			}
		protected:
			std::unique_ptr<Geometry> mGeometry;
			std::unique_ptr<Material> mMaterial;
			BindingBridge mBindingBridge;
		};
	}
}

