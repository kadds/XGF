#pragma once
#include "Defines.hpp"
#include <DirectXMath.h>
#include "Interpolator\Interpolator.hpp"
#include <memory>
namespace XGF
{
	/*
	*/
	class Action
	{
	public:
		Action();
		virtual ~Action();
		//返回True ：已经结束Action
		bool Tick(Point * out, float deltaTime);
		static std::unique_ptr<Action> Make(Point & to, float duration, bool isRelative,std::shared_ptr<Interpolator> interpolator)
		{
			std::unique_ptr<Action> action = std::make_unique<Action>();
			action->mDuration = duration;
			action->mTo = to;
			action->mInterpolator = interpolator;
			action->mIsRelative = isRelative;
			return action;
		}
		void SetFrom(Point &from);
	protected:
		std::shared_ptr<Interpolator> mInterpolator;
		Point mTo;
		Point mFrom;
		Point mFromDelta;
		float mDuration;
		float mTime;
		bool mIsRelative;
	private:
		DISALLOW_COPY_AND_ASSIGN(Action);
	};


}
