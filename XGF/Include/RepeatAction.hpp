#pragma once
#include "CompositeAction.hpp"
class RepeatAction :
	public CompositeAction
{
public:
	RepeatAction();
	~RepeatAction();
	void SetNum(int n) { mNum = n; }
	virtual bool Tick(float time) override;

	virtual void Reset()  override;
private:
	int mNum;
	int mPassNum;
};

