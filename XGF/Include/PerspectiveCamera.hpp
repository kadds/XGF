#pragma once
#include "Camera.hpp"
namespace XGF
{
	/*
	透视投影摄像机
	*/
	class PerspectiveCamera :
		public Camera
	{
	public:
		PerspectiveCamera();

		~PerspectiveCamera();
		virtual void UpdataProject(int cx, int cy);
		virtual void Updata();
		// 左右 
		void Strafe(float units);
		// 上下 
		void Fly(float units);
		// 前后
		void Walk(float units);

		void Yaw(float angle);

		void Pitch(float angle);


		void RotateY(float angle);
		//平移
		void Translation(float x, float y, float z);
		//设置视角大小 ：  弧度
		void SetFovAngle(float va);
		//获取视角大小 ：  弧度
		float GetFovAngle() { return mFovAngle; }
		
	protected:
		float mFovAngle;

	};

};
