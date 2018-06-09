#pragma once
#include "Defines.hpp"
#include <memory>
#include "Action.hpp"
#include "Actions.hpp"
namespace XGF
{
	/*
	* ×ª»»
	*/
	class Transform
	{
	public:
		Point mRotationOrigin;
		Point mScaleOrigin;
		Point4 mScaleOriginRotation;

		Point mRotation;
		Point mScale;
		Point mTranslation;
	private:
		Actions mActions;
	public:
		Transform();
		void SetRotationAngle(Point angle);
		void SetScale(Point p);
		void SetTranslation(Point translation);
		void UpdateAction(float dt);
		void SetScaleOrigin(Point &p) { mScaleOrigin = p; }
		void SetRotationOrigin(Point &p) { mRotationOrigin = p; }
		SM::Matrix GetMatrix();
		void SetMatrix(SM::Matrix matrix);
		void AddScaleAction(std::unique_ptr<Action> action);
		void AddRotationAction(std::unique_ptr<Action> action);
		void AddTranslationAction(std::unique_ptr<Action> action);

		void SetChangeFlag();
	private:
		DirectX::XMFLOAT4X4 mMatrix;
		bool mIsChange;
	};
}