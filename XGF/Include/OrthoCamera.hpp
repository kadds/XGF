#pragma once
#include "Defines.hpp"
#include "Camera.hpp"
namespace XGF
{
	class OrthoCamera :
		public Camera
	{
	public:
		OrthoCamera();
		~OrthoCamera();
		virtual void UpdateProject(int cx, int cy);
		void SetScaleFactor(float scalefactor);
		float GetScaleFactor();
	private:
		float mScaleFactor;
	};

};
