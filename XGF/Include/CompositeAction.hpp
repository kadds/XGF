#pragma once
#include "Defines.hpp"
#include "Action.hpp"
#include <vector>
#include <algorithm>
class CompositeAction :
	public Action
{
public:
	CompositeAction();
	virtual ~CompositeAction();
	virtual bool Tick(float time) override = 0;
	CompositeAction * GetParent() { return parent; }
	void SetParent(CompositeAction * p) { parent = p; }
	void AddAction(std::unique_ptr<Action> action);
	virtual void Reset() override;
	virtual void SetTarget(ActionInterface * target);
protected:
	
	std::vector<std::unique_ptr<Action>> actions;
	CompositeAction * parent;
};

