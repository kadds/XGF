#pragma once
#include "Event.hpp"
namespace XGF
{
	class PerspectiveCamera;
	class TrackballCameraController
	{
	public:
		TrackballCameraController(PerspectiveCamera * camera);
		void HandleEvents(const Event & ev, int cx, int cy);
		void Rotate(float dx, float dy);
		void Zoom(float dx, float dy);
		void Move(float dx, float dy);
		void UpdateTargetFromCamera();
		void SetZoomSpeed(float speed)
		{
			mZoomSpeed = speed;
		}
		void SetAllSpeed(float speed)
		{
			mZoomSpeed = speed;
			mMoveSpeed = speed;
			mRotateSpeed = speed;
		}
		float GetZoomSpeed()
		{
			return mZoomSpeed;
		}
		void SetTarget(Point & target)
		{
			mTarget = target;
		}

	private:
		PerspectiveCamera * mCamera;
		bool mIsDown;
		Position mLastPosition;
		float mZoomSpeed = .01, mMoveSpeed = .01f, mRotateSpeed = .01f;
		Point mTarget{ 0.f,0.f,0.f };
		bool mReverse;
		enum  class OperatorType
		{
			None,
			Rotate,
			Zoom,
			Move
		} mOperatorType = OperatorType::None;
	};
}