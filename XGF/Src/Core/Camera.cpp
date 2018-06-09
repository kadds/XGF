#include "../../Include/Camera.hpp"
namespace XGF 
{
	Camera::Camera() :mMaxDistance(1000.f), mMinDistance(0.00001f), mPos(0.0f, 0.0f, .0f), mRotation(SM::Quaternion::Identity)
	{
	}

	Camera::~Camera()
	{
	}

	void Camera::GetCameraMatrix(WVPMatrix & wvp, const SM::Matrix * pro)
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

	void Camera::Update()
	{
		auto k = SM::Matrix::CreateFromQuaternion(mRotation);

		DirectX::XMVECTOR look = k.Backward();
		DirectX::XMVECTOR up = k.Up();

		SM::Vector3 vfoward{ 2 * (mRotation.x*mRotation.z + mRotation.w * mRotation.y)
			,2 * (mRotation.y*mRotation.z - mRotation.w * mRotation.x)
			,1 - 2 * (mRotation.x*mRotation.x + mRotation.y * mRotation.y) };
		SM::Vector3 vup{ 2 * (mRotation.x*mRotation.y - mRotation.w * mRotation.z)
			,1 - 2 * (mRotation.x*mRotation.x + mRotation.z * mRotation.z)
			,2 * (mRotation.y*mRotation.z + mRotation.w * mRotation.x) };
		SM::Vector3 vleft{ 1-  2 * (mRotation.y*mRotation.y + mRotation.z * mRotation.z)
			,2 * (mRotation.x*mRotation.y + mRotation.w * mRotation.z)
			,2 * (mRotation.x*mRotation.z - mRotation.w * mRotation.y) };

		SM::Vector3 qfoward = mRotation * SM::Vector3::Backward;
		SM::Vector3 qup = mRotation * SM::Vector3::Up;

		DirectX::XMStoreFloat4x4(&viewMatrix, DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&mPos), look, up));
	}

	void Camera::SetPosition(const Point & pos)
	{
		mPos = pos;
	}

	Point Camera::GetForward()
	{
		return SM::Matrix::CreateFromQuaternion(mRotation).Backward();
	}
	Point Camera::GetUp()
	{
		return SM::Matrix::CreateFromQuaternion(mRotation).Up();
	}
	Point Camera::GetRight()
	{
		return  SM::Matrix::CreateFromQuaternion(mRotation).Right();
	}
	void Camera::LookAt(Point eye, Point targetv, Point upv)
	{
		mPos = eye;

		SM::Matrix mat;
		SM::Vector3 up = upv , forward = targetv - eye;
		
		SM::Vector3 right = up.Cross(forward);
		up = forward.Cross(right);
		up.Normalize();
		forward.Normalize();
		right.Normalize();
		mat.Up(up);
		mat.Backward(forward);
		mat.Right(right);
		mRotation = SM::Quaternion::CreateFromRotationMatrix(mat);
	}
	void Camera::LookTo(Point eye, Point eyeDirection, Point upv)
	{
		mPos = eye;
		SM::Matrix mat;
		SM::Vector3 up = upv, forward = eyeDirection;

		SM::Vector3 right = up.Cross(forward);
		up = forward.Cross(right);
		up.Normalize();
		forward.Normalize();
		right.Normalize();
		mat.Up(up);
		mat.Backward(forward);
		mat.Right(right);
		mRotation = SM::Quaternion::CreateFromRotationMatrix(mat);
	}
}
