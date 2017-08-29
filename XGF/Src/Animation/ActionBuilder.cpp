#include "..\..\Include\ActionBuilder.hpp"
#include "..\..\Include\MoveToAction.hpp"
#include "..\..\Include\ScaleToAction.hpp"
#include "..\..\Include\RotateToAction.hpp"
#include "..\..\Include\ChangeColorAction.hpp"
#include "..\..\Include\CompositeAction.hpp"
#include "..\..\Include\SequenceAction.hpp"
#include "..\..\Include\ParallelAction.hpp"
#include "..\..\Include\DelayAction.hpp"
#include "..\..\Include\RepeatAction.hpp"
#include "..\..\Include\AlphaAction.hpp"
#include "..\..\Include\Defines.hpp"
ActionBuilder::ActionBuilder() :now(nullptr), usingCompositeAction(false)
{
}


ActionBuilder::~ActionBuilder()
{
}

void ActionBuilder::EndBuild(std::unique_ptr<Action> & act)
{
	if (now != nullptr)
	{
		act = std::move(nowptr);
	}
	else
		act = std::move(mAction);
	delete this;
}

ActionBuilder & ActionBuilder::BeginBuild()
{
	return *this;
}

ActionBuilder & ActionBuilder::MoveTo(float x, float y, float z, float duration, std::shared_ptr<Interpolator> interpolator)
{
	AddAction(MoveToAction::Make(Point(x, y, z), duration, interpolator));
	return *this;
}

ActionBuilder & ActionBuilder::MoveBy(float x, float y, float z, float duration, std::shared_ptr<Interpolator> interpolator)
{
	auto p = MoveToAction::Make(Point(x, y, z), duration, interpolator);
	p->SetRelativeMode(true);
	AddAction(std::move(p));
	return *this;
}

ActionBuilder & ActionBuilder::ScaleTo(float x, float y, float z, float duration, std::shared_ptr<Interpolator> interpolator)
{
	AddAction(ScaleToAction::Make(Point(x, y, z), duration, interpolator));
	return *this;
}

ActionBuilder & ActionBuilder::ScaleBy(float x, float y, float z, float duration, std::shared_ptr<Interpolator> interpolator)
{
	auto p = ScaleToAction::Make(Point(x, y, z), duration, interpolator);
	p->SetRelativeMode(true);
	AddAction(std::move(p));
	return *this;
}

ActionBuilder & ActionBuilder::RotateTo(float x, float y, float z, float duration, std::shared_ptr<Interpolator> interpolator)
{
	AddAction(RotateToAction::Make(Point(x, y, z), duration, interpolator));
	return *this;
}

ActionBuilder & ActionBuilder::RotateBy(float x, float y, float z, float duration, std::shared_ptr<Interpolator> interpolator)
{
	auto p = RotateToAction::Make(Point(x, y, z), duration, interpolator);
	p->SetRelativeMode(true);
	AddAction(std::move(p));
	return *this;
}

ActionBuilder & ActionBuilder::ChangeColorTo(float x, float y, float z, float a, float duration, std::shared_ptr<Interpolator> interpolator)
{
	AddAction(ChangeColorAction::Make(Color(x, y, z, a), duration, interpolator));
	return *this;
}

ActionBuilder & ActionBuilder::ChangeColorBy(float x, float y, float z, float a, float duration, std::shared_ptr<Interpolator> interpolator)
{
	auto p = ChangeColorAction::Make(Color(x, y, z, a), duration, interpolator);
	p->SetRelativeMode(true);
	AddAction(std::move(p));
	return *this;
}

ActionBuilder & ActionBuilder::AlphaTo(float a, float duration, std::shared_ptr<Interpolator> interpolator)
{
	AddAction(AlphaAction::Make(a, duration, interpolator));
	return *this;
}

ActionBuilder & ActionBuilder::AlphaBy(float a, float duration, std::shared_ptr<Interpolator> interpolator)
{
	auto p = AlphaAction::Make(a, duration, interpolator);
	p->SetRelativeMode(true);
	AddAction(std::move(p));
	return *this;
}

ActionBuilder & ActionBuilder::ParallelActionTo()
{
	auto p = std::make_unique<ParallelAction>();
	p->SetParent(now);
	auto ptr = p.get();
	if (nowptr != nullptr)
		now->AddAction(std::move(p));
	else
	{
		nowptr = std::move(p);
	}
	now = ptr;
	usingCompositeAction = true;
	
	return *this;
}

ActionBuilder & ActionBuilder::SequenceActionTo()
{
	auto p = std::make_unique<SequenceAction>();
	p->SetParent(now);
	auto ptr = p.get();
	if(nowptr != nullptr)
		now->AddAction(std::move(p));
	else
	{
		nowptr = std::move(p);
	}
	now = ptr;
	usingCompositeAction = true;
	return *this;
}

ActionBuilder & ActionBuilder::RepeatActionTo(int num)
{
	auto act = std::make_unique<RepeatAction>();
	act->SetNum(num);
	act->SetParent(now);
	auto ptr = act.get();
	if (nowptr != nullptr)
		now->AddAction(std::move(act));
	else
	{
		nowptr = std::move(act);
	}
	now = ptr;
	usingCompositeAction = true;
	return *this;
}

ActionBuilder & ActionBuilder::DelayActionTo(float time)
{
	AddAction(DelayAction::Delay(time));
	return *this;
}

ActionBuilder & ActionBuilder::BackUp()
{
	if (now->GetParent() != nullptr)
	{
		now = now->GetParent();
	}
	else
	{
		now = nullptr;
		usingCompositeAction = false;
	}
	return *this;
}

ActionBuilder & ActionBuilder::Builder()
{
	return *new ActionBuilder();
}

void ActionBuilder::AddAction(std::unique_ptr<Action> act)
{
	if (nowptr == nullptr)
	{
		mAction = std::move(act);
	}
	else
		now->AddAction(std::move(act));
}
