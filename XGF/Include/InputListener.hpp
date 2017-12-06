#pragma once
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


	class InputListener
	{
	public:
		InputListener() {};
		~InputListener() {};
		virtual void OnMouseDowm(const MousePoint &mp, int pk) = 0;
		virtual void OnMouseUp(const MousePoint &mp, int pk) = 0;
		virtual void OnMouseMove(const MousePoint &mm, int pk) = 0;
		virtual void OnKeyDowm(Key k) = 0;
		virtual void OnKeyUp(Key k) = 0;
	};
	typedef std::function<void(const MousePoint &mm, int pk)> OnClickListener;
	typedef std::function<void(const MousePoint &mm, int pk)> OnMouseDowmListener;
	typedef std::function<void(const MousePoint &mm, int pk)> OnMouseUpListener;
	typedef std::function<void(const MousePoint &mm, int pk)> OnMouseMoveListener;


}
