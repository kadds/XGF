#include "..\..\Include\SceneAnimation.hpp"



SceneAnimation::SceneAnimation():pos(0,0,0),angle(0,0,0),scale(1,1,1),color(0,0,0,1),alpha(1)
{
}


SceneAnimation::~SceneAnimation()
{
}

void SceneAnimation::BeginAnimation()
{
	mActions.Start();
}

void SceneAnimation::Updata(float time)
{
	mActions.Updata(time);
}

XMMATRIX SceneAnimation::GetMatrix() 
{ 
	Point op(0,0,0);
	Point sp(0, 0, 0);
	return DirectX::XMMatrixTransformation2D(
		DirectX::XMLoadFloat3(&sp),
		0,
		DirectX::XMLoadFloat3(&scale),
		DirectX::XMLoadFloat3(&op),
		angle.z,
		DirectX::XMLoadFloat3(&pos)
	);
}

