#include "../../Include/OrthoCamera.hpp"

namespace XGF
{
	OrthoCamera::OrthoCamera() : mScaleFactor(1.f)
	{

	}


	OrthoCamera::~OrthoCamera()
	{
	}

	void OrthoCamera::UpdateProject(int cx, int cy)
	{
		DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&mPos),
			DirectX::XMLoadFloat3(&mLook), DirectX::XMLoadFloat3(&mUp)));
		DirectX::XMStoreFloat4x4(&proMatrix, DirectX::XMMatrixOrthographicOffCenterLH(0, (cx)* mScaleFactor,
			(cy)* mScaleFactor, 0, mMinDistance, mMaxDistance));
	}
	void OrthoCamera::Update()
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
