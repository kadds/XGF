#pragma once
#include "Defines.hpp"
#include "Font.hpp"
#include "Rectangle.hpp"
#include <algorithm>
#include <functional>
namespace XGF
{
	typedef std::function<bool(int, wchar_t, Shape::Rectangle * rc, const PosSize * ps)> LayoutCallbackFunction;
	//return true break function;
	typedef std::function<bool(int, wchar_t, Position * p)> PenMoveCallback;
	/*
		Compute layout info
	*/
	class TextLayoutShaper
	{
	public:
		TextLayoutShaper();
		~TextLayoutShaper();
		Position MeasureStringSize(Font & font, wchar_t * str);
		int DoLayouShaper(const string & str, const Shape::Rectangle & rc, Font & font,
			LayoutCallbackFunction fun, PenMoveCallback movec = nullptr);
	private:
		Shape::Rectangle mRc;
	};


}
