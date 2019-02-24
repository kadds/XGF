#pragma once
#include "Defines.hpp"
#include "Camera.hpp"
namespace XGF
{
	class PerspectiveCamera :
		public Camera
	{
	public:
		PerspectiveCamera();

		~PerspectiveCamera();
		virtual void UpdateProject(int cx, int cy);
		// Left Right
		void Strafe(float units);
		// Up Down
		void Fly(float units);
		// Front Back
		void Walk(float units);

		void Yaw(float angle);

		void Pitch(float angle);

		void Roll(float angle);


		void RotateY(float angle);
		
		void Translation(float x, float y, float z);

		void SetFovAngle(float va);

		float GetFovAngle() const { return mFovAngle; }
		
	protected:
		float mFovAngle;
		
	};

};
