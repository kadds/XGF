#pragma once
#include "Defines.hpp"
#include "Action.hpp"
#include "Actions.hpp"
namespace XGF
{

	class Transform
	{
	private:
		Point mRotationOrigin;
		Point mScaleOrigin;
		Point4 mScaleOriginRotation;

		Point mRotation;
		Point mScale;
		Point mTranslation;
	public:
		Transform();
		void SetRotationAngle(const Point & angle);
		void SetScale(const Point & p);
		void SetTranslation(const Point & translation);

		const Point & GetTranslate() const;
		const Point & GetScale() const;
		const Point & GetRotation() const;

		Point & GetTranslate();
		Point & GetScale();
		Point & GetRotation();

		void SetScaleOrigin(Point &p) { mScaleOrigin = p; }
		void SetRotationOrigin(Point &p) { mRotationOrigin = p; }
		SM::Matrix GetMatrix();
		void SetMatrix(SM::Matrix matrix);

		void TranslateToX(float d);
		void TranslateX(float d);
		void TranslateToY(float d);
		void TranslateY(float d);
		void TranslateToZ(float d);
		void TranslateZ(float d);
		void SetChangeFlag();
		bool IsChange() const;

		float GetTranslateX() const;
		float GetTranslateY() const;
		float GetTranslateZ() const;

		float GetScaleX() const;
		float GetScaleY() const;
		float GetScaleZ() const;

		float GetRotationX() const;
		float GetRotationY() const;
		float GetRotaionZ() const;
	private:
		DirectX::XMFLOAT4X4 mMatrix;
		bool mIsChange;
	};
}