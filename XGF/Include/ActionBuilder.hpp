#pragma once
#include "Defines.hpp"
#include <memory>
namespace XGF
{
	class CompositeAction;
	class Action;
	class Interpolator;
	/*

	*/
	class ActionBuilder
	{
	public:
		ActionBuilder();
		~ActionBuilder();
		void EndBuild(std::unique_ptr<Action> & act);
		ActionBuilder & BeginBuild();
		ActionBuilder & MoveTo(float x, float y, float z, float time, std::shared_ptr<Interpolator> interpolator);
		ActionBuilder & MoveBy(float x, float y, float z, float time, std::shared_ptr<Interpolator> interpolator);

		ActionBuilder & ScaleTo(float x, float y, float z, float time, std::shared_ptr<Interpolator> interpolator);
		ActionBuilder & ScaleBy(float x, float y, float z, float time, std::shared_ptr<Interpolator> interpolator);

		ActionBuilder & RotateTo(float x, float y, float z, float time, std::shared_ptr<Interpolator> interpolator);
		ActionBuilder & RotateBy(float x, float y, float z, float time, std::shared_ptr<Interpolator> interpolator);

		ActionBuilder & ChangeColorTo(float x, float y, float z, float a, float time, std::shared_ptr<Interpolator> interpolator);
		ActionBuilder & ChangeColorBy(float x, float y, float z, float a, float time, std::shared_ptr<Interpolator> interpolator);
		ActionBuilder & AlphaTo(float a, float time, std::shared_ptr<Interpolator> interpolator);
		ActionBuilder & AlphaBy(float a, float time, std::shared_ptr<Interpolator> interpolator);
		//下面函数会改变Action包含结构
		ActionBuilder & ParallelActionTo();
		ActionBuilder & SequenceActionTo();
		ActionBuilder & RepeatActionTo(int num);
		ActionBuilder & DelayActionTo(float time);
		//返回当前Actiojn结构的上一级
		ActionBuilder & BackUp();

		static ActionBuilder & Builder();
	protected:
		CompositeAction  * now;
		std::unique_ptr<CompositeAction> nowptr;
		std::unique_ptr<Action> mAction;
		bool usingCompositeAction;

		void AddAction(std::unique_ptr<Action> act);
	};


}
