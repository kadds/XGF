#include "../../Include/PerspectiveCamera.hpp"

namespace XGF
{
	using namespace DirectX;
	PerspectiveCamera::PerspectiveCamera() :mFovAngle(DirectX::XM_PI / 3.f)
	{
		mPos = DirectX::XMFLOAT3(.0f, .0f, .0f);
		mUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		mRight = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		mLook = DirectX::XMFLOAT3(0.f, 0.f, 1.f);
	}


	PerspectiveCamera::~PerspectiveCamera()
	{
	}

	void PerspectiveCamera::UpdateProject(int cx, int cy)
	{
		DirectX::XMStoreFloat4x4(&proMatrix, DirectX::XMMatrixPerspectiveFovLH(mFovAngle, cx / float(cy), mMinDistance, mMaxDistance));
	}

	void PerspectiveCamera::Update()
	{
		DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&mLook);
		DirectX::XMVECTOR up, right, pos;
		up = DirectX::XMLoadFloat3(&mUp);
		right = DirectX::XMLoadFloat3(&mRight);
		pos = DirectX::XMLoadFloat3(&mPos);
		up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(look, right));
		right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, look));
		look = DirectX::XMVector3Normalize(look);
		DirectX::XMStoreFloat3(&mUp, up);
		DirectX::XMStoreFloat3(&mRight, right);
		DirectX::XMStoreFloat3(&mLook, look);

		DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookToLH(pos, look, up ));
		//DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, -1)),
		//	DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 0)), DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 1, 0))));
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
		XMMATRIX rotation = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&mUp), angle);

		DirectX::XMStoreFloat3(&mRight, DirectX::XMVector3TransformNormal(XMLoadFloat3(&mRight), rotation));
		DirectX::XMStoreFloat3(&mLook, DirectX::XMVector3TransformNormal(XMLoadFloat3(&mLook), rotation));
	}
	void PerspectiveCamera::Pitch(float angle)
	{
		XMMATRIX rotation = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&mRight), angle);

		DirectX::XMStoreFloat3(&mUp, DirectX::XMVector3TransformNormal(XMLoadFloat3(&mUp), rotation));
		DirectX::XMStoreFloat3(&mLook, DirectX::XMVector3TransformNormal(XMLoadFloat3(&mLook), rotation));
	}
	void PerspectiveCamera::RotateY(float angle)
	{
		XMMATRIX rotation = DirectX::XMMatrixRotationY(angle);

		DirectX::XMStoreFloat3(&mRight, XMVector3TransformNormal(DirectX::XMLoadFloat3(&mRight), rotation));
		DirectX::XMStoreFloat3(&mUp, XMVector3TransformNormal(DirectX::XMLoadFloat3(&mUp), rotation));
		DirectX::XMStoreFloat3(&mLook, XMVector3TransformNormal(DirectX::XMLoadFloat3(&mLook), rotation));
	}
	void PerspectiveCamera::Translation(float x, float y, float z)
	{
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(x, y, z);
		DirectX::XMStoreFloat3(&mPos, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&mPos), T));
		DirectX::XMStoreFloat3(&mUp, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&mUp), T));
		DirectX::XMStoreFloat3(&mLook, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&mLook), T));
	}

	void PerspectiveCamera::SetFovAngle(float va)
	{
		mFovAngle = va;
	}

};

