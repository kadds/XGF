#include "..\..\Include\Mesh.hpp"

namespace XGF
{
	namespace Shape
	{

		Mesh::Mesh(Geometry * geometry, Material * material): mGeometry(geometry), mMaterial(material)
		{

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
	}
}


