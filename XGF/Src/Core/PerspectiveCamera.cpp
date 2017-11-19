#include "../../Include/PerspectiveCamera.hpp"



PerspectiveCamera::PerspectiveCamera():mFovAngle(DirectX::XM_PI / 3.f),mFixYAxis(false)
{
	mPos =  DirectX::XMFLOAT4(10.0f,10.0f,10.0f,0.f);
	mUp = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.f);
	mRight = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.f);
	mLook = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 0.f);
}


PerspectiveCamera::~PerspectiveCamera()
{
}

void PerspectiveCamera::UpdataProject(int cx, int cy)
{
	DirectX::XMStoreFloat4x4(&proMatrix, DirectX::XMMatrixPerspectiveFovLH(mFovAngle, cx / float(cy), mMinDistance, mMaxDistance));
}

void PerspectiveCamera::Updata()
{
	DirectX::XMStoreFloat4(&mLook, DirectX::XMVector3Normalize(DirectX::XMLoadFloat4(&mLook)));
	//正交 归一化
	if (mFixYAxis)
	{
		mUp.x = 0.f;
		mUp.y = 1.f;
		mUp.z = 0.f;
		mUp.w = 0.f;
	}
	else
	{
		DirectX::XMStoreFloat4(&mUp, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat4(&mLook), DirectX::XMLoadFloat4(&mRight))));
	}
	DirectX::XMStoreFloat4(&mRight, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat4(&mUp), DirectX::XMLoadFloat4(&mLook))));

	DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat4(&mPos)
		, DirectX::XMLoadFloat4(&mLook), DirectX::XMLoadFloat4(&mUp)));

}
void PerspectiveCamera::Walk(float units)
{
	mPos.x += mLook.x * units;
	mPos.y += mLook.y * units;
	mPos.z += mLook.z * units;
}

void PerspectiveCamera::Strafe(float units)
{
	mPos.x += mRight.x * units;
	mPos.y += mRight.y * units;
	mPos.z += mRight.z * units;
}
void PerspectiveCamera::Fly(float units)
{
	mPos.x += mUp.x * units;
	mPos.y += mUp.y * units;
	mPos.z += mUp.z * units;
}

void PerspectiveCamera::Yaw(float angle)
{
	PitchYawRoll(0.f, angle, 0.f);
}
void PerspectiveCamera::Pitch(float angle)
{
	PitchYawRoll(angle, 0.f, 0.f);
}
void PerspectiveCamera::Roll(float angle)
{
	PitchYawRoll(0.f, 0.f, angle);
}
void PerspectiveCamera::PitchYawRoll(float angle1, float angle2, float angle3)
{
	DirectX::XMMATRIX T = DirectX::XMMatrixRotationRollPitchYaw(angle1,angle2,angle3);
	
	DirectX::XMStoreFloat4(&mPos, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&mPos), T));
	DirectX::XMStoreFloat4(&mUp, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&mUp), T));
	DirectX::XMStoreFloat4(&mLook, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&mLook), T));
}
void PerspectiveCamera::Translation(float x, float y, float z)
{
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(x, y, z);
	DirectX::XMStoreFloat4(&mPos, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&mPos), T));
	DirectX::XMStoreFloat4(&mUp, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&mUp), T));
	DirectX::XMStoreFloat4(&mLook, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&mLook), T));
}

void PerspectiveCamera::SetFovAngle(float va)
{
	mFovAngle = va;
}

void PerspectiveCamera::SetPos(DirectX::XMFLOAT4 & pos)
{
	mPos = pos;
}

void PerspectiveCamera::FixYAxis(bool isFix)
{
	mFixYAxis = isFix;
}
