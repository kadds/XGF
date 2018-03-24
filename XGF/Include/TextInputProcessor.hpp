#pragma once
#include <tchar.h>
#include "Defines.hpp"
#include "Rectangle.hpp"
#include <string>
#include <vector>
#include <functional>
namespace XGF
{
	struct PointI
	{
		int x;
		int y;
	};
	class TextRenderer;
	/*
	具有接受输入Text 和渲染Text的类
	*/
	class TextInputProcessor
	{
	public:
		TextInputProcessor();
		TextInputProcessor(const string & text);
		~TextInputProcessor();
		void SetCaretPosInText(int posx);
		void AppendInputStr(wchar_t * str, int count);
		void AppendInputStr(wchar_t str);
		void DelInputStr();
		void RenderText(Color & color);
		void BackUp();
		void Delete();
		void CaretToLeft();
		void CaretToRight();
		void CaretToUp();
		void CaretToDowm();
		int GetCaretPosInText() { return mCaretPos; }

		void GetCaretProperty(float &x, float &y, int &size);
		const wchar_t* GetText();

		void OnFocus(bool isFocus);

		void SetInnerRectangle(std::function<void(Shape::Rectangle &)> fun) { mGetInnerRectangle = fun; };
		void SetTextRenderer(TextRenderer * textRenderer) { mTextRenderer = textRenderer; };
		void SetOnFocusListener(std::function<void(bool)> fun) { mOnFocusListener = fun; };

		void SetText(const string & text);
	private:
		string mInputString;
		bool layoutCallBack(int i, wchar_t ch, Position * p, bool c, Position & temp);
		int mCaretPos;
		int mFirstCharPos;
		Position mCaretPosition;
		std::pair<int, Position> mCallBackBuffer;
		std::function<void(bool)> mOnFocusListener;
		std::function<void(Shape::Rectangle &)> mGetInnerRectangle;
		TextRenderer * mTextRenderer;

	};

}
