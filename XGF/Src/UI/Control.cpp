#include "../../Include/Control.hpp"

Control::Control():Actor()
{
}

Control::~Control()
{
}

XMMATRIX Control::CalculateMatrix() const {
	Point mx;
	GetShape()->GetPosition(mx);
	Point op = mx;
	op.x += mRotationOrigin.x;
	op.y += mRotationOrigin.y;
	op.z += mRotationOrigin.z;
	Point sp = mx;
	sp.x += mScaleOrigin.x;
	sp.y += mScaleOrigin.y;
	sp.z += mScaleOrigin.z;
	return DirectX::XMMatrixTransformation2D(
		DirectX::XMLoadFloat3(&sp),
		0,
		DirectX::XMLoadFloat3(&mScale),
		DirectX::XMLoadFloat3(&op),
		mRotation.z,
		DirectX::XMLoadFloat3(&mTranslation)
	);
}


