#include "..\..\Include\Mesh.hpp"

namespace XGF
{
	namespace Shape
	{

		Mesh::Mesh(Geometry * geometry, Material * material): mGeometry(geometry), mMaterial(material)
		{
			mBindingBridge.AddBinder(geometry->mPolygon);
			mBindingBridge.AddBinder(material->CreateBinders(*geometry));
		}

		Mesh::~Mesh()
		{
		}
		Material * Mesh::GetMaterial()
		{
			return mMaterial;
		}
		Geometry * Mesh::GetGeometry()
		{
			return mGeometry;
		}
		BindingBridge & Mesh::GetBindingBridge()
		{
			return mBindingBridge;
		}
	}
}


