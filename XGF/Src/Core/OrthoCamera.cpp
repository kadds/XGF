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
		Camera::UpdateProject(cx, cy);
		
		DirectX::XMStoreFloat4x4(&proMatrix, DirectX::XMMatrixOrthographicOffCenterLH(0, (cx)* mScaleFactor,
			(cy)* mScaleFactor, 0, mMinDistance, mMaxDistance));
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
