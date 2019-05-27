#include "./../../Include/TrackballCameraController.hpp"
#include "./../../Include/PerspectiveCamera.hpp"
#include "./../../Include/InputDefinitions.hpp"
#include "./../../Include/Context.hpp"
#include "./../../Include/InputManager.hpp"
#include "./../../Include/XGFramework.hpp"

namespace XGF
{
	TrackballCameraController::TrackballCameraController(PerspectiveCamera * camera) : mLastPosition(0, 0), mIsDown(false)
	{
		mCamera = camera;
	}
	void TrackballCameraController::HandleEvents(const Event & ev, int cx, int cy)
	{
		if (ev.mEventType != EventGroupType::Mouse) return;
		auto& inputManager = Context::Current().QueryFramework().GetInputManager();
		Input::MouseKey pressKey;
		switch (ev.GetMouseEventId())
		{
			case MouseEventId::MouseMove:
				if (!mIsDown) break;
				{
					Position cp{ static_cast<float>(ev.GetDataMousePosition(0)), static_cast<float>(ev.GetDataMousePosition(1)) };
					float dx = mLastPosition.x - cp.x;
					float dy = mLastPosition.y - cp.y;
					mLastPosition = cp;
					if (mOperatorType == OperatorType::Rotate)
						Rotate(-dx * mRotateSpeed, dy * mRotateSpeed);
					else if (mOperatorType == OperatorType::Move)
						Move(dx * mMoveSpeed, dy * mMoveSpeed);
					break;
				}
			case MouseEventId::MouseUp:
				if (!mIsDown) break;
				mIsDown = false;
				mOperatorType = OperatorType::None;
				break;
			case MouseEventId::MouseDown:
				if (mIsDown) break;
				mIsDown = true;
				mLastPosition = { (float) inputManager.GetMouseContent().GetPosX(), (float) inputManager.GetMouseContent().GetPosY() };
				pressKey = ev.GetData<Input::MouseKey>(0);
				if (pressKey == Input::MouseKey::Left)
				{
					mOperatorType = OperatorType::Rotate;
				}
				else if (pressKey == Input::MouseKey::Right)
				{
					mOperatorType = OperatorType::Move;
				}
				else
				{
					mOperatorType = OperatorType::None;
				}
				break;
			case MouseEventId::MouseWheel:
				this->Zoom(ev.GetDataMousePosition(0) * mZoomSpeed, 0);
				break;
			default:
				break;
		}
	}
	void TrackballCameraController::Rotate(float dx, float dy)
	{
		SM::Matrix mat = SM::Matrix::CreateFromAxisAngle(mCamera->GetRight(), dy);
		SM::Matrix mat2 = SM::Matrix::CreateFromAxisAngle(mCamera->GetUp(), dx);
		SM::Vector3 pos = mCamera->GetPosition();
		pos = DirectX::XMVector3Transform(pos, mat);
		pos = DirectX::XMVector3Transform(pos, mat2);

		SM::Vector3 mt ;
		if (mReverse)
		{
			mt = pos - mTarget;
		}
		else
		{
			mt = mTarget - pos;
		}
		//mTarget = mTarget / mt.Length();

		mCamera->LookAt(pos, mTarget, mCamera->GetUp());
	}

	void TrackballCameraController::Zoom(float dx, float dy)
	{
		SM::Vector3 offset = mCamera->GetForward() * (dx + dy);
		SM::Vector3 pos = mCamera->GetPosition() + offset;

		if ((mTarget - pos).Dot(mCamera->GetForward()) <= 0)
		{
			mReverse = true;
		}
		else
		{
			mReverse = false;
		}
		mCamera->LookAt(pos, mTarget, mCamera->GetUp());
	}
	void TrackballCameraController::Move(float dx, float dy)
	{
		SM::Vector3 offset = mCamera->GetRight() * -dx;
		SM::Vector3 pos = mCamera->GetPosition() + offset;
		mTarget += offset;

		offset = mCamera->GetUp() * dy;
		pos += offset;
		mTarget += offset;

		mCamera->LookAt(pos, mTarget, mCamera->GetUp());
	}
	void TrackballCameraController::UpdateTargetFromCamera()
	{

	}
}