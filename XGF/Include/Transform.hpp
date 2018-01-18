#pragma once
#include "Defines.hpp"
#include "ActionInterface.hpp"
#include <memory>
#include "Action.hpp"
#include "Actions.hpp"
namespace XGF
{
	/*
	* ×ª»»
	*/
	class Transform : public ActionInterface
	{
	public:
		Point mRotationOrigin;
		Point mScaleOrigin;

		Point mRotation;
		Point mScale;
		Point mTranslation;

		void SetAction(std::unique_ptr<Action> action);
		void StartAction();
		void StopAction();

		void OnActionCompleted();

		virtual void OnPositionChange(const Point & p, int ID) { mTranslation = p; };
		virtual void OnAngleChange(const Point & p, int ID) { mRotation = p; };
		virtual void OnScaleChange(const Point & p, int ID) { mScale = p; };
		virtual void OnColorChange(const Color & p, int ID) {};
		virtual void OnAlphaChange(float a, int ID) {};

		virtual void GetPosition(Point & p, int ID) const { p = mTranslation; };
		virtual void GetAngle(Point & p, int ID) const { p = mRotation; };
		virtual void GetScale(Point & p, int ID) const { p = mScale; };
		virtual void GetColor(Color & p, int ID) const {};
		virtual void GetAlpha(float & p, int ID) const {};
	public:
		Transform();
		void SetRotationAngle(Point angle);
		void SetScale(Point p);
		void SetTranslation(Point translation);

		void SetScaleOrigin(Point &p) { mScaleOrigin = p; }
		void SetRotationOrigin(Point &p) { mRotationOrigin = p; }
		XMMATRIX GetMatrix();

		void SetChangeFlag();
	private:
		DirectX::XMFLOAT4X4 mMatrix;
		bool mIsChange;
		Actions mActions;
	};
}