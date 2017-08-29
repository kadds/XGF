#include "..\..\Include\TextInputInterface.hpp"
#include "..\..\Include\TextRenderer.hpp"
#include "../../Include/Log.hpp"


TextInputInterface::TextInputInterface():mCursorPos(0)
{
	mInputString.clear();
	mFirstCharPos = 0;
}


TextInputInterface::~TextInputInterface()
{
}

void TextInputInterface::SetCursorPos(int pos)
{
	mCursorPos = pos;
}


void TextInputInterface::AppendInputStr(wchar_t * str, int count)
{
	mInputString.append(str, count);
	//mCursorPos += count;
}
void TextInputInterface::AppendInputStr(wchar_t str)
{
	mInputString.insert(mCursorPos,1,str);
	mCursorPos++;
	SetCaretPos(mCursorPosition.x, mCursorPosition.y);
}
void TextInputInterface::DelInputStr()
{
	mInputString.clear();
}


void TextInputInterface::RenderText(const XMMATRIX * matrix, Shape::Rectangle & rc, Color & color)
{
	Position u = mTextRenderer->DrawString(mInputString.c_str(), mFirstCharPos, -1, color, &rc, matrix);
	//mCursorPosition.x = u.x + rc->mPolygon.mPoint[0].x;
	//mCursorPosition.y = u.y + rc->mPolygon.mPoint[0].y;
	//rc->mPolygon.Translation(u.x , u.y, 0);
	//mTextRenderer->DrawString(mInputString.c_str(), mCursorPos +1 , -1, Color(0.0, 0.0, 0.0, 1.0f), rc, &d);
	//rc->mPolygon.Translation(-u.x, -u.y, 0);
	//mTextRenderer->DrawString(mInputString, mCursorPos, -1, Color(0.0, 0.0, 0.0, 1.0f), rc, &d);
	//rc->mPolygon->Translation(-u, 0, 0);
}

void TextInputInterface::BackUp()
{
	if (mCursorPos > 0)
	{
		mInputString.erase(mCursorPos - 1 , 1);
		if (mCursorPos >= 1)
			mCursorPos--;
		SetCaretPos(mCursorPosition.x, mCursorPosition.y);
	}
}

void TextInputInterface::Delete()
{
	if (mInputString.length() > 0 && mCursorPos <= mInputString.length() - 1)
	{
		mInputString.erase(mCursorPos, 1);
		SetCaretPos(mCursorPosition.x, mCursorPosition.y);
	}
}

void TextInputInterface::CursorToLeft()
{
	if (mCursorPos > 0)
		mCursorPos--;
	SetCaretPos(mCursorPosition.x, mCursorPosition.y);
}

void TextInputInterface::CursorToRight()
{
	mCursorPos++;
	if (mCursorPos > mInputString.length())
	{
		mCursorPos = static_cast<int>(mInputString.length());
	}
	SetCaretPos(mCursorPosition.x, mCursorPosition.y);
}

void TextInputInterface::CursorToUp()
{
	//TODO::未完成，需计算排版信息
}

void TextInputInterface::CursorToDowm()
{
	//TODO::未完成
}

const wchar_t * TextInputInterface::GetText()
{
	return mInputString.c_str();
}
