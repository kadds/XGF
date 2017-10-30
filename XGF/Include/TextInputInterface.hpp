#pragma once
#include <tchar.h>
#include "TextRenderInterface.hpp"
#include "Defines.hpp"
#include "Rectangle.hpp"
#include <string>
struct PointI
{
	int x;
	int y;
};
/*
具有接受输入Text 和渲染Text的类
*/
class TextInputInterface : public TextRenderInterface
{
public:
	TextInputInterface();
	~TextInputInterface();
	void SetCaretPosInText(int posx);
	void AppendInputStr(wchar_t * str, int count);
	void AppendInputStr(wchar_t str);
	void DelInputStr();
	void RenderText(const XMMATRIX * matrix, Shape::Rectangle & rc, Color & color);
	void BackUp();
	void Delete();
	void CaretToLeft();
	void CaretToRight();
	void CaretToUp();
	void CaretToDowm();
	int GetCaretPosInText() { return mCaretPos; }

	void GetCaretProperty(int &x, int &y, int &size);
	const wchar_t* GetText();
	//框架调用
	virtual void OnForce(bool isForce) = 0;
private:
	std::wstring mInputString;
	int mCaretPos;
	int mFirstCharPos;
	PointI mCaretPosition;
};

