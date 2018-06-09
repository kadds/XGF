#include "..\..\Include\SceneAnimation.hpp"

namespace XGF
{
	SceneAnimation::SceneAnimation() :pos(0, 0, 0), angle(0, 0, 0), scale(1, 1, 1), color(1, 1, 1, 1), alpha(0)
	{
	}


	SceneAnimation::~SceneAnimation()
	{
	}

	void SceneAnimation::BeginAnimation()
	{
	}

	void SceneAnimation::SetAction(std::unique_ptr<Action> ac)
	{
		isend = false;
	}

	SM::Matrix SceneAnimation::GetMatrix()
	{
		Point op(0, 0, 0);
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


}

