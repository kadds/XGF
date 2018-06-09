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
		void GetCameraMatrix(WVPMatrix & wvp, const SM::Matrix * pro = nullptr);
		virtual void UpdateProject(int cx, int cy)
		{
			mAspectRatio = cx / (float) cy;
			mLastWidth = cx;
			mLastHeight = cy;
		};
		void Update();

		void SetMaxDistance(float f) { mMaxDistance = f; }
		void SetMinDistance(float f) { mMinDistance = f; }

		//…Ë÷√…„œÒª˙Œª÷√
		void SetPosition(const Point & pos);
		Point GetPosition() { return mPos; };

		Point GetForward();
		Point GetUp();
		Point GetRight();
		float GetAspectRatio()
		{
			return mAspectRatio;
		};
		SM::Quaternion GetRotation() 
		{
			return mRotation;
		}
		void SetRotation(SM::Quaternion & r)
		{
			mRotation = r;
		}
		void LookAt(Point eye, Point target, Point up);
		void LookTo(Point eye, Point eyeDirection, Point up);
	protected:
		SM::Matrix viewMatrix;
		SM::Matrix proMatrix;
		float mMaxDistance;
		float mMinDistance;
		int mLastWidth;
		int mLastHeight;

		Point mPos;

		float mAspectRatio;
		SM::Quaternion mRotation;
		
	};


}
