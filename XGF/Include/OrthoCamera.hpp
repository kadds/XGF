#pragma once
#include "Camera.hpp"
/*
正交投影摄像机
*/
class OrthoCamera :
	public Camera
{
public:
	OrthoCamera();
	~OrthoCamera();
	virtual void UpdataProject(int cx, int cy);
	virtual void Updata();
	void SetScaleFactor(float scalefactor);
	float GetScaleFactor();
private:
	float mScaleFactor;
};

