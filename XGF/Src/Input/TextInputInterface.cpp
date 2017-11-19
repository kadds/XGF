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
	TextRenderer * tx = GetTextRenderer();
	if (mCaretPos == 0)
	{
		tx->DrawStringRtPosition(mInputString.c_str() + mFirstCharPos, color, &rc, matrix, 0);
		mCaretPosition.x = rc.mPolygon.mPoint[0].x + 0.8f;
		mCaretPosition.y = rc.mPolygon.mPoint[0].y + 0.f;
	}
	else if (mCaretPos > 0)
	{
		Position u = tx->DrawStringRtPosition(mInputString.c_str() + mFirstCharPos, color, &rc, matrix, mCaretPos);
		mCaretPosition.x = u.x + rc.mPolygon.mPoint[0].x + 0.8f;
		mCaretPosition.y = u.y + rc.mPolygon.mPoint[0].y + 0.f;
	}
	else
	{
		//do nothing
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
inline bool equalcirca(float a, float b, float k)
{
	return fabs(a - b) < k;
}
bool TextInputInterface::layoutCallBack(int i, wchar_t ch, Position * p, bool c, Position & temp)//c == true is Down
{
	TextRenderer * tx = GetTextRenderer();
	auto h = tx->GetFontSize() + 1;
	if (i < 0) {
		if (mCallBackBuffer.first == -1)
			mCallBackBuffer.first = 5;
	}
	if ((c ? (p->y - temp.y <= h
		&&  p->y - temp.y > 0) : (temp.y - p->y <= h && temp.y - p->y > 0)))
	{
		
		if (p->x - temp.x > h / 2.0)
		{
			return true;
		}
		mCallBackBuffer.first = i;
		mCallBackBuffer.second = *p;

	}
	return false;
}
void TextInputInterface::CaretToUp()
{
	TextRenderer * tx = GetTextRenderer();
	Shape::Rectangle rc;
	GetInerBox(rc);
	mCallBackBuffer.first = -1;
	mCallBackBuffer.second = Position(0,0);
	Position temp;
	temp.x = mCaretPosition.x - rc.mPolygon.mPoint[0].x - 0.8f;
	temp.y = mCaretPosition.y - rc.mPolygon.mPoint[0].y;
	tx->GetLayoutShaper()->DoLayouShaper(mInputString.c_str() + mFirstCharPos, rc, *tx->GetFont(), nullptr,
		std::bind(&TextInputInterface::layoutCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, false, temp));
	if (mCallBackBuffer.first > 0 && mCallBackBuffer.first < mInputString.size())
		mCaretPos = mCallBackBuffer.first;
}

void TextInputInterface::CaretToDowm()
{
	TextRenderer * tx = GetTextRenderer();
	Shape::Rectangle rc;
	GetInerBox(rc);
	mCallBackBuffer.first = -1;
	mCallBackBuffer.second = Position(0, 0);
	Position temp;
	temp.x = mCaretPosition.x - rc.mPolygon.mPoint[0].x - 0.8f;
	temp.y = mCaretPosition.y - rc.mPolygon.mPoint[0].y;
	tx->GetLayoutShaper()->DoLayouShaper(mInputString.c_str() + mFirstCharPos, rc, *tx->GetFont(), nullptr,
		std::bind(&TextInputInterface::layoutCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, true, temp));
 	if(mCallBackBuffer.first > 0 && mCallBackBuffer.first < mInputString.size())
		mCaretPos = mCallBackBuffer.first;
}

void TextInputInterface::GetCaretProperty(float & x, float & y, int & size)
{
	x = mCaretPosition.x;
	y = mCaretPosition.y;
	size = GetTextRenderer()->GetFontSize();
}

const wchar_t * TextInputInterface::GetText()
{
	return mInputString.c_str();
}
