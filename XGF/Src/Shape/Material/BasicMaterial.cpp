#include "..\..\..\Include\Material\BasicMaterial.hpp"
#include "..\..\..\Include\Geometry\Geometry.hpp"
namespace XGF
{
	using XGF::ConstantData;
	namespace Shape
	{
		BasicMaterial::BasicMaterial(const SM::Color & color): baseColor(color)
		{
			mShaders = ConstantData::GetInstance().GetPCShaders();
		}
		BindingBridge BasicMaterial::CreateBinders(const Geometry & geometry)
		{
			BindingBridge bbr;
			auto ccb = std::make_shared<PolygonPleConstantColorBinder>(baseColor, geometry.mPolygon->mCount);
			bbr.AddBinder(ccb);
			return bbr;
		}
	}
}