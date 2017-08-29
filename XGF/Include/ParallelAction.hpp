#pragma once
#include "CompositeAction.hpp"

class ParallelAction :
	public CompositeAction
{
public:
	ParallelAction();
	~ParallelAction();
	virtual bool Tick(float time) override;
};

