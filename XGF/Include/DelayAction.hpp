#pragma once
#include "Defines.hpp"
#include "Action.hpp"
namespace XGF
{
	class DelayAction :
		public Action
	{
	public:
		DelayAction();
		~DelayAction();
		static std::unique_ptr<DelayAction> Delay(float time);
		virtual bool Tick(float passTime);
	private:
	};

}

