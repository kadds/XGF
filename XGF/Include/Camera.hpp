#pragma once
#include "Defines.hpp"
class Camera
{
public:
	Camera();
	~Camera();
	virtual void GetCameraMatrix(WVPMatrix & wvp, const DirectX::XMFLOAT4X4 * pro = nullptr);
	virtual void UpdataSize(int width, int height) = 0;
	virtual void Updata() = 0;

	void SetMaxDistance(float f) { mMaxDistance = f; }
	void SetMinDistance(float f) { mMinDistance = f; }

protected:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 proMatrix;
	float mMaxDistance;
	float mMinDistance;

	int mLastWidth;
	int mLastHeight;
};

