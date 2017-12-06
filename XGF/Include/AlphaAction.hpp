#pragma once
#include "Action.hpp"
namespace XGF
{
	class AlphaAction :
		public Action
	{
	public:
		AlphaAction();
		~AlphaAction();
		static std::unique_ptr<AlphaAction> Make(float c, float time, std::shared_ptr<Interpolator> interpolator);
		virtual bool Tick(float passTime) override;

	private:
		float mTargetAlpha;
		float mStartAlpha;
	};


}
