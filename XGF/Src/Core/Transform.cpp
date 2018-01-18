#include "../../Include/Transform.hpp"
namespace XGF
{
	Transform::Transform() : mScale(1.f, 1.f, 1.f), mRotation(0.0f, 0.0f, 0.0f), mTranslation(0.f, 0.f, 0.f),
		mRotationOrigin(0.f, 0.f, 0.f), mScaleOrigin(0.f, 0.f, 0.f), mIsChange(true)
	{
		DirectX::XMStoreFloat4x4(&mMatrix, DirectX::XMMatrixIdentity());
		mActions.SetOnActionCompletedListener(std::bind(&Transform::OnActionCompleted, this));
		//mActions.Updata(deltaTime);
	}
	void Transform::SetRotationAngle(Point angle)
	{
		mRotation = angle;
		mIsChange = true;
	}
	void Transform::SetScale(Point p)
	{
		mScale = p;
		mIsChange = true;
	}
	void Transform::SetAction(std::unique_ptr<Action> action)
	{
		mActions.SetAction(std::move(action), this);
	}
	void Transform::StartAction()
	{
		mActions.Start();
	}
	void Transform::StopAction()
	{
		mActions.Stop();
	}


	void Transform::OnActionCompleted()
	{

	}

	XMMATRIX Transform::GetMatrix()
	{
		if (!mIsChange) return DirectX::XMLoadFloat4x4(&mMatrix);
		//3D æÿ’Ûº∆À„
		Point mx = mTranslation;

		Point op = mTranslation;
		op.x += mRotationOrigin.x;
		op.y += mRotationOrigin.y;
		op.z += mRotationOrigin.z;
		auto matrix = DirectX::XMMatrixTransformation(
			DirectX::XMLoadFloat3(&mx),
			DirectX::XMLoadFloat3(&mRotation),
			DirectX::XMLoadFloat3(&mScale),
			DirectX::XMLoadFloat3(&op),
			DirectX::XMLoadFloat3(&mRotation),
			DirectX::XMLoadFloat3(&mTranslation)
		);;
		DirectX::XMStoreFloat4x4(&mMatrix, matrix);
		mIsChange = false;
		return matrix;
	}
	void Transform::SetChangeFlag()
	{
		mIsChange = true;
	}
	void Transform::SetTranslation(Point translation) {
		mTranslation = translation;
		mIsChange = true;
	}
}
