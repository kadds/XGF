#pragma once
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
		void Pause();
		void Resume();
		bool IsPause() const;
		bool IsStart() const;
		void AddAction(std::unique_ptr<Action> act);
	private:
		bool mIsActionBegan;
		bool mIsPause;
		std::vector<std::unique_ptr<Action>> mActions;

	};


}
