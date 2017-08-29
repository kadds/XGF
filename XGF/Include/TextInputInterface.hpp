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
	void SetCursorPos(int pos);
	void AppendInputStr(wchar_t * str, int count);
	void AppendInputStr(wchar_t str);
	void DelInputStr();
	void RenderText(const XMMATRIX * matrix, Shape::Rectangle & rc, Color & color);
	void BackUp();
	void Delete();
	void CursorToLeft();
	void CursorToRight();
	void CursorToUp();
	void CursorToDowm();
	int GetCursorPos() { return mCursorPos; }
	const wchar_t* GetText();
	//框架调用
	virtual void OnForce(bool isForce) = 0;
private:
	std::wstring mInputString;
	int mCursorPos;
	int mFirstCharPos;
	PointI mCursorPosition;
};

