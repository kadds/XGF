#pragma once
#include "Camera.hpp"
/*
透视投影摄像机
*/
class PerspectiveCamera :
	public Camera
{
public:
	PerspectiveCamera();
	
	~PerspectiveCamera();
	virtual void UpdataProject(int cx, int cy);
	virtual void Updata();
	// 左右 
	void Strafe(float units);
	// 上下 
	void Fly(float units);
	// 前后
	void Walk(float units);

	void Yaw(float angle);

	void Pitch(float angle); 

	void PitchYawRoll(float angle1, float angle2, float angle3);

	void Roll(float angle);
	//平移
	void Translation(float x, float y, float z);
	//设置视角大小 ：  弧度
	void SetFovAngle(float va);
	//获取视角大小 ：  弧度
	float GetFovAngle(){return mFovAngle; }
	//设置摄像机位置
	void SetPos(DirectX::XMFLOAT4 & pos);
	//固定Y轴向上
	void FixYAxis(bool isFix);
private:
	DirectX::XMFLOAT4 mPos;
	DirectX::XMFLOAT4 mRight;
	DirectX::XMFLOAT4 mLook;
	DirectX::XMFLOAT4 mUp;
	float mFovAngle;
	bool mFixYAxis;
	
};

