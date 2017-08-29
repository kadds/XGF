#pragma once
#include "CompositeAction.hpp"
class SequenceAction :
	public CompositeAction
{
public:
	SequenceAction();
	~SequenceAction();
	virtual bool Tick(float time) override;
	virtual void Reset()  override;
private:
	int nowAction;
};

