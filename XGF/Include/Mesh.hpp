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
			Mesh(Geometry * geometry, Material * material);
			~Mesh();
			Material * GetMaterial();
			Geometry * GetGeometry();
			BindingBridge & GetBindingBridge();
		private:
			Geometry * mGeometry;
			Material * mMaterial;
			BindingBridge mBindingBridge;
		};
	}
}

