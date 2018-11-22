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
		Point4 f0(0, 0, 0, 0);
		//3D æÿ’Ûº∆À„
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
	void Transform::AddScaleAction(std::unique_ptr<Action> action)
	{
		mActions.AddAction(mScale, std::move(action));
	}
	void Transform::AddRotationAction(std::unique_ptr<Action> action)
	{
		mActions.AddAction(mRotation, std::move(action));
	}
	void Transform::AddTranslationAction(std::unique_ptr<Action> action)
	{
		mActions.AddAction(mTranslation, std::move(action));
	}
	void Transform::SetChangeFlag()
	{
		mIsChange = true;
	}
	void Transform::SetTranslation(const Point & translation) {
		mTranslation = translation;
		mIsChange = true;
	}
	void Transform::UpdateAction(float dt)
	{
		if (mActions.IsStart())
		{
			mActions.Update(dt);
			mIsChange = true;
		}
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
