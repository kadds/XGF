#pragma once
#include "Camera.hpp"
/*
͸��ͶӰ�����
*/
class PerspectiveCamera :
	public Camera
{
public:
	PerspectiveCamera();
	
	~PerspectiveCamera();
	virtual void UpdataSize(int width, int height);
	virtual void Updata();
	virtual void GetCameraMatrix(WVPMatrix & wvp, DirectX::XMFLOAT4X4 * pro = nullptr);
	// ���� 
	void Strafe(float units);
	// ���� 
	void Fly(float units);
	// ǰ��
	void Walk(float units);

	void Yaw(float angle);

	void Pitch(float angle); 

	void PitchYawRoll(float angle1, float angle2, float angle3);

	void Roll(float angle);
	//ƽ��
	void Translation(float x, float y, float z);
	//�����ӽǴ�С ��  ����
	void SetFovAngle(float va);
	//��ȡ�ӽǴ�С ��  ����
	float GetFovAngle(){return mFovAngle; }
	//���������λ��
	void SetPos(DirectX::XMFLOAT4 & pos);
	//�̶�Y������
	void FixYAxis(bool isFix);
private:
	DirectX::XMFLOAT4 mPos;
	DirectX::XMFLOAT4 mRight;
	DirectX::XMFLOAT4 mLook;
	DirectX::XMFLOAT4 mUp;
	float mFovAngle;
	bool mFixYAxis;
	
};

