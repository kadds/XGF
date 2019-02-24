#pragma once
#include "Defines.hpp"
#include "PerspectiveCamera.hpp"
namespace XGF
{
	class FPSCamera :public  PerspectiveCamera
	{
	public:
		FPSCamera();
		~FPSCamera();
		virtual void Update();
	};
}


