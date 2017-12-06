#pragma once
#include "Defines.hpp"
#include "Action.hpp"
namespace XGF
{
	class ChangeColorAction :
		public Action
	{
	public:
		ChangeColorAction();
		~ChangeColorAction();
		static std::unique_ptr<ChangeColorAction> Make(Color & c, float time, std::shared_ptr<Interpolator> interpolator);
		virtual bool Tick(float passTime) override;

	private:
		Color mTargetColor;
		Color mStartColor;
	};


}
