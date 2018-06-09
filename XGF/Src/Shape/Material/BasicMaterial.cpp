#include "..\..\..\Include\Material\BasicMaterial.hpp"

namespace XGF
{
	using XGF::ConstantData;
	namespace Shape
	{
		BasicMaterial::BasicMaterial(const SM::Color & color): baseColor(color)
		{
			mShaders = ConstantData::GetInstance().GetPCShaders();
		}
	}
}