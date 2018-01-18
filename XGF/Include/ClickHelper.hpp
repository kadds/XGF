#pragma once
#include "Defines.hpp"
#include <vector>
#include <functional>
namespace XGF
{
	struct MousePoint
	{
		int x;
		int y;
	};
	// DIK_?
	typedef unsigned Key;
	class ClickHelper
	{
	public:
		ClickHelper();
		~ClickHelper();
		

		typedef std::function<void(const MousePoint &mm, int mouseButton)> OnClickListener;
		void OnMouseDowm(const MousePoint & mp, bool isInTarget);
		void OnMouseUp(const MousePoint & mp, bool isInTarget);
		void OnMouseMove(bool isInTarget);
		void AddOnClickListener(OnClickListener l) {
			mOnClickListener.push_back(l);
		}
		int GetState() { return mState; }
	private:
		void OnClick(const MousePoint & mp);
		int mState;
		std::vector<OnClickListener> mOnClickListener;
	};

#define CLICK_STATE_NORMAL 0
#define CLICK_STATE_FORCE 1
#define CLICK_STATE_DOWM 10
}

#define CLICK_STATE_UP 20