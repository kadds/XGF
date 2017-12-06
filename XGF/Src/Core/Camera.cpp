#include "../../Include/Camera.hpp"
namespace XGF 
{
	Camera::Camera() :mMaxDistance(1000.f), mMinDistance(0.001f)
	{
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
		wvp.Transpose();
	}
}
