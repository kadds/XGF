#pragma once
#include "Defines.hpp"
#include "Actions.hpp"
#include <DirectXMath.h>
namespace XGF
{
	class SceneAnimation
	{
	public:
		SceneAnimation();
		~SceneAnimation();
		bool IsEnd() { return isend; };
		void BeginAnimation();
		void SetAction(std::unique_ptr<Action> ac);
		void Update(float time)
		{
			mActions.Update(time);
		}
		Actions * GetActions() { return &mActions; };
		XMMATRIX GetMatrix();
		virtual void OnPositionChange(const Point & p, int ID) { pos = p; };
		virtual void OnAngleChange(const Point & p, int ID) { angle = p; };
		virtual void OnScaleChange(const Point & p, int ID) { scale = p; };
		virtual void OnColorChange(const Color & p, int ID) { color = p; };
		virtual void OnAlphaChange(float a, int ID) { alpha = a; };

		virtual void GetPosition(Point & p, int ID) const { p = pos; };
		virtual void GetAngle(Point & p, int ID) const { p = angle; };
		virtual void GetScale(Point & p, int ID) const { p = scale; };
		virtual void GetColor(Color & p, int ID) const { p = color; };
		virtual void GetAlpha(float & p, int ID) const { p = alpha; };


	private:
		void ok() { isend = true; };
		Actions mActions;
		DirectX::XMFLOAT4X4 mMatrix;
		Point pos;
		Color color;
		Point scale;
		Point angle;
		float alpha;
		bool isend;
	};


}
