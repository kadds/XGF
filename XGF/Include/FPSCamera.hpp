#pragma once
#include "PerspectiveCamera.hpp"
class FPSCamera :public  PerspectiveCamera
{
public:
	FPSCamera();
	~FPSCamera();
	virtual void Updata();
};

