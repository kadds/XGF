#include "..\..\Include\FPSCamera.hpp"



FPSCamera::FPSCamera()
{
}


FPSCamera::~FPSCamera()
{
}

void FPSCamera::Updata()
{
	mUp.x = 0;
	mRight.y = 0;
	PerspectiveCamera::Updata();
}
