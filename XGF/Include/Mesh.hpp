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
				: mGeometry(std::move(geometry)), mMaterial(std::move(material)), dFlag(true)
			{
				Generate();
			}
			~Mesh() = default;

			std::unique_ptr<Material> LockMaterial()
			{
				return std::move(mMaterial);
			}

			BindingBridge & GetBindingBridge()
			{
				if (dFlag)
				{
					dFlag = false;
					Generate();
				}
				return mBindingBridge;
			}
			void UnLockMaterial(std::unique_ptr<Material> m)
			{
				dFlag = true;
				mMaterial = std::move(m);
			}
			Material * GetMaterial() const
			{
				return mMaterial.get();
			}
			std::unique_ptr<Geometry> LockGeometry()
			{
				return std::move(mGeometry);
			}
			void UnLockGeometry(std::unique_ptr<Geometry> m)
			{
				dFlag = true;
				mGeometry = std::move(m);
			}
			Geometry * GetGeometry() const
			{
				return mGeometry.get();
			}
			std::vector<Texture *> GetTextureList() const
			{
				return mMaterial->GetTextureBindingList();
			}
		private:
			void Generate()
			{
				if (mBindingBridge.Count() > 0)
				{
					mBindingBridge.Clear();
				}
				mBindingBridge.AddBinder(mGeometry->GetMapBinder());
			}
		protected:
			std::unique_ptr<Geometry> mGeometry;
			std::unique_ptr<Material> mMaterial;
			BindingBridge mBindingBridge;
			bool dFlag;
		};
	}
}

