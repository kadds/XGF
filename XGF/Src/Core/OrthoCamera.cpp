#include "../../Include/OrthoCamera.hpp"

namespace XGF
{
	OrthoCamera::OrthoCamera() : mScaleFactor(1.f)
	{

	}


	OrthoCamera::~OrthoCamera()
	{
	}

	void OrthoCamera::UpdataProject(int cx, int cy)
	{
		//DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, -1)),
			DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 1, 0))));
		DirectX::XMStoreFloat4x4(&proMatrix, DirectX::XMMatrixOrthographicOffCenterLH(0, (cx)* mScaleFactor,
			(cy)* mScaleFactor, 0, mMinDistance, mMaxDistance));
		//DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixOrthographicLH(width, height, mMinDistance, mMaxDistance));
	}
	void OrthoCamera::Updata()
	{
	}

	inline void OrthoCamera::SetScaleFactor(float scalefactor)
	{
		mScaleFactor = scalefactor;
	}

	inline float OrthoCamera::GetScaleFactor()
	{
		return mScaleFactor;
	}

}
