#include "../../Include/OrthoCamera.hpp"



OrthoCamera::OrthoCamera(): mScaleFactor(1.f)
{
}


OrthoCamera::~OrthoCamera()
{
}

void OrthoCamera::UpdataSize(int width, int height)
{
	DirectX::XMStoreFloat4x4(&proMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixOrthographicLH(static_cast<float>(width) * mScaleFactor, static_cast<float>(height) * mScaleFactor, mMinDistance, mMaxDistance));
	mLastHeight = height;
	mLastWidth = width;
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
