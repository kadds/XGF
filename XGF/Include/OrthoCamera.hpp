#pragma once
#include "Camera.hpp"
/*
����ͶӰ�����
*/
class OrthoCamera :
	public Camera
{
public:
	OrthoCamera();
	~OrthoCamera();
	virtual void UpdataSize(int width, int height);
	virtual void Updata();
	void SetScaleFactor(float scalefactor);
	float GetScaleFactor();
private:
	float mScaleFactor;
};
