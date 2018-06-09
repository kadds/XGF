#include "../../Include/PerspectiveCamera.hpp"

namespace XGF
{
	using namespace DirectX;
	PerspectiveCamera::PerspectiveCamera() :mFovAngle(DirectX::XM_PI / 3.f)
	{
		mPos = DirectX::XMFLOAT3(.0f, .0f, .0f);
	}


	PerspectiveCamera::~PerspectiveCamera()
	{
	}

	void PerspectiveCamera::UpdateProject(int cx, int cy)
	{
		Camera::UpdateProject(cx, cy);
		DirectX::XMStoreFloat4x4(&proMatrix, DirectX::XMMatrixPerspectiveFovLH(mFovAngle, cx / float(cy), mMinDistance, mMaxDistance));
	}
	void PerspectiveCamera::Walk(float units)
	{
		mPos += SM::Matrix::CreateFromQuaternion(mRotation).Forward() * units;
	}

	void PerspectiveCamera::Strafe(float units)
	{
		mPos += SM::Matrix::CreateFromQuaternion(mRotation).Right() * units;
	}
	void PerspectiveCamera::Fly(float units)
	{
		mPos += SM::Matrix::CreateFromQuaternion(mRotation).Up() * units;
	}

	void PerspectiveCamera::Yaw(float angle)
	{
		mRotation *=  SM::Quaternion::CreateFromYawPitchRoll(angle, 0 , 0);
	}
	void PerspectiveCamera::Pitch(float angle)
	{
		mRotation *= SM::Quaternion::CreateFromYawPitchRoll(0, angle, 0);
	}
	void PerspectiveCamera::Roll(float angle)
	{
		mRotation *= SM::Quaternion::CreateFromYawPitchRoll(0, 0, angle);
	}
	void PerspectiveCamera::RotateY(float angle)
	{
		XMMATRIX rotation = DirectX::XMMatrixRotationY(angle);
		mRotation *= SM::Quaternion::CreateFromRotationMatrix(rotation);
	}
	void PerspectiveCamera::Translation(float x, float y, float z)
	{
		mPos += SM::Vector3(x, y, z);
	}

	void PerspectiveCamera::SetFovAngle(float va)
	{
		mFovAngle = va;
	}

};

