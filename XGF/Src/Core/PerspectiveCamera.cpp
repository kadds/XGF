#include "../../Include/PerspectiveCamera.hpp"

namespace XGF
{

	PerspectiveCamera::PerspectiveCamera() :mFovAngle(DirectX::XM_PI / 3.f)
	{
		mPos = DirectX::XMFLOAT3(0.0f, 10.0f, 10.0f);
		mUp = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		mRight = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		mLook = DirectX::XMFLOAT3(1.f, 1.f, 1.f);
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
		DirectX::XMVECTOR look = DirectX::XMLoadFloat3(&mLook);
		DirectX::XMVECTOR up, right, pos;
		up = DirectX::XMLoadFloat3(&mUp);
		right = DirectX::XMLoadFloat3(&mRight);
		pos = DirectX::XMLoadFloat3(&mPos);

		up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(look, right));
		right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, look));
		look = DirectX::XMVector3Normalize(look);

		float x = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(pos, right));
		float y = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(pos, up));
		float z = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(pos, look));
		DirectX::XMStoreFloat3(&mLook, look);
		DirectX::XMStoreFloat3(&mUp, up);
		DirectX::XMStoreFloat3(&mRight, right);
		viewMatrix(0, 0) = mRight.x;    viewMatrix(0, 1) = mUp.x;   viewMatrix(0, 2) = mLook.x; viewMatrix(0, 3) = 0;
		viewMatrix(1, 0) = mRight.y;    viewMatrix(1, 1) = mUp.y;   viewMatrix(1, 2) = mLook.y; viewMatrix(1, 3) = 0;
		viewMatrix(2, 0) = mRight.z;    viewMatrix(2, 1) = mUp.z;   viewMatrix(2, 2) = mLook.z; viewMatrix(2, 3) = 0;
		viewMatrix(3, 0) = x;            viewMatrix(3, 1) = y;        viewMatrix(3, 2) = z;         viewMatrix(3, 3) = 1;


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

	void PerspectiveCamera::SetPos(DirectX::XMFLOAT3 & pos)
	{
		mPos = pos;
	}
};

