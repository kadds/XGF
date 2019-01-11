#pragma once
#include "Defines.hpp"
#include "Interpolator\Interpolator.hpp"
namespace XGF
{
	/*
	*/
	class Action
	{
	public:
		Action();
		virtual ~Action();
		//锟斤拷锟斤拷True 锟斤拷锟窖撅拷锟斤拷锟斤拷Action
		bool Tick(Point * out, float deltaTime);
		static std::unique_ptr<Action> Make(const Point & to, float duration, bool isRelative,std::shared_ptr<Interpolator> interpolator)
		{
			std::unique_ptr<Action> action = std::make_unique<Action>();
			action->mDuration = duration;
			action->mTo = to;
			action->mInterpolator = interpolator;
			action->mIsRelative = isRelative;
			return action;
		}
		void SetFrom(const Point &from);
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
