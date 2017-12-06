#pragma once
#include "CompositeAction.hpp"
namespace XGF
{
	class ParallelAction :
		public CompositeAction
	{
	public:
		ParallelAction();
		~ParallelAction();
		virtual bool Tick(float time) override;
	};

}

