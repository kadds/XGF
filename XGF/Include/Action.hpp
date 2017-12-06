#pragma once
#include "Defines.hpp"
#include <DirectXMath.h>
#include "Interpolator\Interpolator.hpp"
#include "ActionInterface.hpp"
#include <memory>
namespace XGF
{
	/*
	��Ϊ���࣬ ������Ϊ
	����Action����ʹ������ָ�����
	*/
	class Action
	{
	public:
		Action();
		virtual ~Action();
		//����True ���Ѿ�����Action
		virtual bool Tick(float time) = 0;
		virtual void Reset();
		void SetRelativeMode(bool RelativeMode) {
			mIsRelativeMode = RelativeMode;
		}
		virtual void SetTarget(ActionInterface * target) { mTarget = target; }
		bool IsComplete() { return mComplete; }
		bool IsBegan() { return mBegan; }
	protected:
		float mTime;
		bool mBegan;
		bool mComplete;
		bool mIsRelativeMode;
		std::shared_ptr<Interpolator> mInterpolator;
		ActionInterface * mTarget;
		int mID;
	private:
		DISALLOW_COPY_AND_ASSIGN(Action);
	};


}
