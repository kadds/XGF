#pragma once
#include <tchar.h>
#include "Defines.hpp"
#include "Rectangle.hpp"
#include <string>
#include <vector>
struct PointI
{
	int x;
	int y;
};
class TextRenderer;
/*
具有接受输入Text 和渲染Text的类
*/
class TextInputInterface
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
	virtual TextRenderer * GetTextRenderer() = 0;
	void CaretToUp();
	void CaretToDowm();
	int GetCaretPosInText() { return mCaretPos; }

	void GetCaretProperty(float &x, float &y, int &size);
	const wchar_t* GetText();
	//框架调用
	virtual void OnForce(bool isForce) = 0;
	virtual void GetInerBox(::Shape::Rectangle & rc) = 0;
private:
	std::wstring mInputString;
	bool layoutCallBack(int i, wchar_t ch, Position * p, bool c, Position & temp);
	int mCaretPos;
	int mFirstCharPos;
	Position mCaretPosition;
	std::pair<int, Position> mCallBackBuffer;
};

