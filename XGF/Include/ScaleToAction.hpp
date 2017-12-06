#pragma once
#include "Action.hpp"
#include "Defines.hpp"
namespace XGF
{
	class ScaleToAction :
		public Action
	{
	public:
		ScaleToAction();
		~ScaleToAction();
		static std::unique_ptr<ScaleToAction> Make(Point & scale, float time, std::shared_ptr<Interpolator> interpolator);
		virtual bool Tick(float passTime) override;
	private:
		Point mTargetScale;
		Point mStartScale;
	};


}
