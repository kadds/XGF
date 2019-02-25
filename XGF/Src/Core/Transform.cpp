#include "../../Include/Transform.hpp"
namespace XGF
{
	Transform::Transform() : mScale(1.f, 1.f, 1.f), mRotation(0.0f, 0.0f, 0.0f), mTranslation(0.f, 0.f, 0.f),
		mRotationOrigin(0.f, 0.f, 0.f), mScaleOrigin(0.f, 0.f, 0.f), mIsChange(true), mScaleOriginRotation(0,0,0,0)
	{
		DirectX::XMStoreFloat4x4(&mMatrix, DirectX::XMMatrixIdentity());
	}
	void Transform::SetRotationAngle(const Point &  angle)
	{
		mRotation = angle;
		mIsChange = true;
	}
	void Transform::SetScale(const Point &  p)
	{
		mScale = p;
		mIsChange = true;
	}
	
	SM::Matrix Transform::GetMatrix()
	{
		if (!mIsChange) return DirectX::XMLoadFloat4x4(&mMatrix);

		auto matrix = DirectX::XMMatrixTransformation(
			DirectX::XMLoadFloat3(&mScaleOrigin),
			DirectX::XMLoadFloat4(&mScaleOriginRotation),
			DirectX::XMLoadFloat3(&mScale),
			DirectX::XMLoadFloat3(&mRotationOrigin),
			DirectX::XMQuaternionRotationMatrix(DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&mRotation))),
			DirectX::XMLoadFloat3(&mTranslation)
		);
		DirectX::XMStoreFloat4x4(&mMatrix, matrix);
		mIsChange = false;
		return matrix;
	}
	void Transform::SetMatrix(SM::Matrix matrix)
	{
		DirectX::XMStoreFloat4x4(&mMatrix ,matrix);
	}

	void Transform::SetChangeFlag()
	{
		mIsChange = true;
	}
	bool Transform::IsChange() const
	{
		return mIsChange;
	}
	float Transform::GetTranslateX() const
	{
		return mTranslation.x;
	}
	float Transform::GetTranslateY() const
	{
		return mTranslation.y;
	}
	float Transform::GetTranslateZ() const
	{
		return mTranslation.z;
	}
	float Transform::GetScaleX() const
	{
		return mScale.x;
	}
	float Transform::GetScaleY() const
	{
		return mScale.y;
	}
	float Transform::GetScaleZ() const
	{
		return mScale.z;
	}
	float Transform::GetRotationX() const
	{
		return mRotation.x;
	}
	float Transform::GetRotationY() const
	{
		return mRotation.y;
	}
	float Transform::GetRotaionZ() const
	{
		return mRotation.z;
	}
	const Point & Transform::GetTranslate() const
	{
		return mTranslation;
	}
	const Point & Transform::GetScale() const
	{
		return mScale;
	}
	const Point & Transform::GetRotation() const
	{
		return mRotation;
	}
	Point & Transform::GetTranslate()
	{
		return mTranslation;
	}
	Point & Transform::GetScale()
	{
		return mScale;
	}
	Point & Transform::GetRotation()
	{
		return mRotation;
	}
	void Transform::SetTranslation(const Point & translation) {
		mTranslation = translation;
		mIsChange = true;
	}
	void Transform::TranslateToX(float d)
	{
		mTranslation.x = d;
		SetChangeFlag();
	}
	void Transform::TranslateToY(float d)
	{
		mTranslation.y = d;
		SetChangeFlag();	
	}
	void Transform::TranslateToZ(float d)
	{
		mTranslation.z = d;
		SetChangeFlag();
	}
	void Transform::TranslateX(float d)
	{
		mTranslation.x += d;
		SetChangeFlag();
	}
	void Transform::TranslateY(float d)
	{
		mTranslation.y += d;
		SetChangeFlag();
	}
	void Transform::TranslateZ(float d)
	{
		mTranslation.z += d;
		SetChangeFlag();
	}


}
