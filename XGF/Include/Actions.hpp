#pragma once
#include <functional>
#include <memory>
#include "Defines.hpp"
#include "Action.hpp"
namespace XGF
{
	class Actions
	{
	public:
		Actions();
		~Actions();

		void Update(float time);

		bool IsStart();
		void AddAction(Point & From, std::unique_ptr<Action> act);
	private:
		bool mIsActionBegan;
		std::vector<std::pair<Point *, std::unique_ptr<Action>>> mAction;

	};


}
