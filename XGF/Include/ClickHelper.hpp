#pragma once
#include "Defines.hpp"
#include "InputListener.hpp"
#include <vector>
namespace XGF
{
	class ClickHelper
	{
	public:
		ClickHelper();
		~ClickHelper();

		void OnMouseDowm(const MousePoint & mp, int c, bool isIn);
		void OnMouseUp(const MousePoint & mp, int c, bool isIn);
		void OnMouseMove(bool isIn);
		void AddOnClickListener(OnClickListener l) {
			mOnClickListener.push_back(l);
		}
		int GetState() { return mState; }
	private:
		void OnClick(const MousePoint & mp, int c);
		int mState;
		std::vector<OnClickListener> mOnClickListener;
	};

#define CLICK_STATE_NORMAL 0
#define CLICK_STATE_FORCE 1
#define CLICK_STATE_DOWM 10
}

#define CLICK_STATE_UP 20