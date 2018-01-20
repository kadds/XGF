#include "..\..\Include\FPSCamera.hpp"


namespace XGF
{
	FPSCamera::FPSCamera()
	{
	}


	FPSCamera::~FPSCamera()
	{
	}

	void FPSCamera::Update()
	{
		mUp.x = 0;
		mRight.y = 0;
		PerspectiveCamera::Update();
	}

}
