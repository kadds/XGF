#pragma once
#include "Defines.hpp"
namespace XGF
{
	class RenderTarget;
	class Camera
	{
	public:
		Camera();
		~Camera();
		void GetCameraMatrix(WVPMatrix & wvp, const XMFLOAT4X4 * pro = nullptr);
		virtual void UpdateProject(int cx, int cy) = 0;
		virtual void Update() = 0;

		void SetMaxDistance(float f) { mMaxDistance = f; }
		void SetMinDistance(float f) { mMinDistance = f; }

		//…Ë÷√…„œÒª˙Œª÷√
		void SetPos(const XMFLOAT3 & pos);

		Point GetPosition() { return mPos; };
	protected:
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 proMatrix;
		float mMaxDistance;
		float mMinDistance;
		int mLastWidth;
		int mLastHeight;

		XMFLOAT3 mPos;
		XMFLOAT3 mRight;
		XMFLOAT3 mLook;
		XMFLOAT3 mUp;
		
	};


}
