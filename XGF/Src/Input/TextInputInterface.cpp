#include "..\..\Include\TextInputInterface.hpp"
#include "..\..\Include\TextRenderer.hpp"
#include "../../Include/Log.hpp"


TextInputInterface::TextInputInterface():mCaretPos(0)
{
	mInputString.clear();
	mFirstCharPos = 0;
}


TextInputInterface::~TextInputInterface()
{
}

void TextInputInterface::SetCaretPosInText(int pos)
{
	mCaretPos = pos;
}


void TextInputInterface::AppendInputStr(wchar_t * str, int count)
{
	mInputString.append(str, count);
	//mCaretPos += count;
}
void TextInputInterface::AppendInputStr(wchar_t str)
{
	mInputString.insert(mCaretPos,1,str);
	mCaretPos++;
}
void TextInputInterface::DelInputStr()
{
	mInputString.clear();
}


void TextInputInterface::RenderText(const XMMATRIX * matrix, Shape::Rectangle & rc, Color & color)
{
	Position u = mTextRenderer->DrawStringRtPosition(mInputString.c_str(), color, &rc, matrix, mCaretPos);
	if (mCaretPos == 0)
	{
		mCaretPosition.x = rc.mPolygon.mPoint[0].x + 1;
		mCaretPosition.y = rc.mPolygon.mPoint[0].y;
	}
	else
	{
		mCaretPosition.x = u.x + rc.mPolygon.mPoint[0].x +  1;
		mCaretPosition.y = u.y + rc.mPolygon.mPoint[0].y;
	}
}

void TextInputInterface::BackUp()
{
	if (mCaretPos > 0)
	{
		mInputString.erase(mCaretPos - 1 , 1);
		if (mCaretPos >= 1)
			mCaretPos--;
	}
}

void TextInputInterface::Delete()
{
	if (mInputString.length() > 0 && mCaretPos <= mInputString.length() - 1)
	{
		mInputString.erase(mCaretPos, 1);
	}
}

void TextInputInterface::CaretToLeft()
{
	if (mCaretPos > 0)
		mCaretPos--;
}

void TextInputInterface::CaretToRight()
{
	mCaretPos++;
	if (mCaretPos > mInputString.length())
	{
		mCaretPos = static_cast<int>(mInputString.length());
	}
}

void TextInputInterface::CaretToUp()
{
	//TODO::未完成，需计算排版信息
}

void TextInputInterface::CaretToDowm()
{
	//TODO::未完成
}

void TextInputInterface::GetCaretProperty(int & x, int & y, int & size)
{
	x = mCaretPosition.x;
	y = mCaretPosition.y;
	size = mTextRenderer->GetFontSize();
}

const wchar_t * TextInputInterface::GetText()
{
	return mInputString.c_str();
}
