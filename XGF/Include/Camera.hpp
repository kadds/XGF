#pragma once
#include "Defines.hpp"
namespace XGF
{
	struct CameraRectangle
	{
		float left;
		float right;
		float bottom;
		float top;
		float nearZ; 
		float farZ;
		CameraRectangle(float left, float right, float bottom, float top, float nearZ, float farZ): left(left), right(right), bottom(bottom), top(top), nearZ(nearZ), farZ(farZ) {  }
		CameraRectangle()
		{
			bottom = -10.f;
			top = 10.f;
			nearZ = 0.1f;
			farZ = 10.f;
			left = -10.f;
			right = 10.f;
		};
	};

	class Camera
	{
	public:
		Camera();
		virtual  ~Camera();
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

		void SetPosition(const Point & pos);
		Point GetPosition() const { return mPos; };

		Point GetForward();
		Point GetUp();
		Point GetRight();
		float GetAspectRatio() const
		{
			return mAspectRatio;
		};
		SM::Quaternion GetRotation() const
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
