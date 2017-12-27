#pragma once
#include "Camera.hpp"
namespace XGF
{
	/*
	͸��ͶӰ�����
	*/
	class PerspectiveCamera :
		public Camera
	{
	public:
		PerspectiveCamera();

		~PerspectiveCamera();
		virtual void UpdataProject(int cx, int cy);
		virtual void Updata();
		// ���� 
		void Strafe(float units);
		// ���� 
		void Fly(float units);
		// ǰ��
		void Walk(float units);

		void Yaw(float angle);

		void Pitch(float angle);


		void RotateY(float angle);
		//ƽ��
		void Translation(float x, float y, float z);
		//�����ӽǴ�С ��  ����
		void SetFovAngle(float va);
		//��ȡ�ӽǴ�С ��  ����
		float GetFovAngle() { return mFovAngle; }
		
	protected:
		float mFovAngle;

	};

};
