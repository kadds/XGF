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
		void GetCameraMatrix(WVPMatrix & wvp, const DirectX::XMFLOAT4X4 * pro = nullptr);
		virtual void UpdateProject(int cx, int cy) = 0;
		virtual void Update() = 0;

		void SetMaxDistance(float f) { mMaxDistance = f; }
		void SetMinDistance(float f) { mMinDistance = f; }

		//…Ë÷√…„œÒª˙Œª÷√
		void SetPos(DirectX::XMFLOAT3 & pos);

		Point GetPosition() { return mPos; };
	protected:
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 proMatrix;
		float mMaxDistance;
		float mMinDistance;
		int mLastWidth;
		int mLastHeight;

		DirectX::XMFLOAT3 mPos;
		DirectX::XMFLOAT3 mRight;
		DirectX::XMFLOAT3 mLook;
		DirectX::XMFLOAT3 mUp;
		
	};


}
