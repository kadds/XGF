#pragma once
#include <functional>
#include <memory>
#include "Defines.hpp"
#include "Action.hpp"
class ActionInterface;
typedef std::function<void(void)> OnActionCompletedListener;
class Actions
{
public:
	Actions();
	~Actions();

	void Updata(float time);
	void SetOnActionCompletedListener(OnActionCompletedListener l) { onActionCompletedListener = l; }
	void Start();
	void Stop();
	bool IsStart();
	void SetAction(std::unique_ptr<Action> action, ActionInterface * ainterface);
private:
	bool mIsActionBegan;
	float mPassedTime;
	std::unique_ptr<Action> mAction;
	OnActionCompletedListener onActionCompletedListener;
};

