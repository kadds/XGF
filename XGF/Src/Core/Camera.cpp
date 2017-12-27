#include "../../Include/Camera.hpp"
namespace XGF 
{
	Camera::Camera() :mMaxDistance(1000.f), mMinDistance(0.00001f)
	{
		mPos = DirectX::XMFLOAT3(0.0f, 0.0f, .0f);
		mUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		mRight = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		mLook = DirectX::XMFLOAT3(0.f, 0.f, 1.f);
	}

	Camera::~Camera()
	{
	}

	void Camera::GetCameraMatrix(WVPMatrix & wvp, const DirectX::XMFLOAT4X4 * pro)
	{
		auto fm = DirectX::XMMatrixIdentity();
		wvp.worldMatrix = fm;
		wvp.viewMatrix = DirectX::XMLoadFloat4x4(&viewMatrix);
		if (pro != nullptr)
			wvp.projMatrix = DirectX::XMLoadFloat4x4(pro) * DirectX::XMLoadFloat4x4(&proMatrix);
		else
			wvp.projMatrix = DirectX::XMLoadFloat4x4(&proMatrix);
		//wvp.Transpose();
	}

	void Camera::SetPos(DirectX::XMFLOAT3 & pos)
	{
		mPos = pos;
	}
}
