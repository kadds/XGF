#pragma once
#include "Defines.hpp"
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
	class Control;
	class ClickHelper
	{
	public:
		ClickHelper();
		~ClickHelper();
		

		typedef std::function<void(Control * source, const MousePoint &mm, int mouseButton)> OnClickListener;
		void OnMouseDown(const MousePoint & mp, bool isInTarget);
		void OnMouseUp(Control * source, const MousePoint & mp, bool isInTarget);
		void OnMouseMove(bool isInTarget);
		void AddOnClickListener(OnClickListener l) {
			mOnClickListener.push_back(l);
		}
		int GetState() { return mState; }
	private:
		void OnClick(Control * source, const MousePoint & mp);
		int mState;
		std::vector<OnClickListener> mOnClickListener;
	};

#define CLICK_STATE_NORMAL 0
#define CLICK_STATE_FORCE 1
#define CLICK_STATE_DOWN 10
}

#define CLICK_STATE_UP 20