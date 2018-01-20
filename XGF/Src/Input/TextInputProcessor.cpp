#include "..\..\Include\TextInputProcessor.hpp"
#include "..\..\Include\TextRenderer.hpp"
#include "../../Include/TextLayoutShaper.hpp"
#include "../../Include/Log.hpp"
namespace XGF
{
	TextInputProcessor::TextInputProcessor() :mCaretPos(0), mGetInnerRectangle(nullptr), mTextRenderer(nullptr)
	{
		mInputString.clear();
		mFirstCharPos = 0;
	}

	TextInputProcessor::TextInputProcessor(wchar_t * text) :mInputString(text), mCaretPos(0), mGetInnerRectangle(nullptr), mTextRenderer(nullptr), mFirstCharPos(0)
	{
	}


	TextInputProcessor::~TextInputProcessor()
	{
	}

	void TextInputProcessor::SetCaretPosInText(int pos)
	{
		mCaretPos = pos;
	}
	void TextInputProcessor::AppendInputStr(wchar_t * str, int count)
	{
		mInputString.append(str, count);
		//mCaretPos += count;
	}
	void TextInputProcessor::AppendInputStr(wchar_t str)
	{
		mInputString.insert(mCaretPos, 1, str);
		mCaretPos++;
	}
	void TextInputProcessor::DelInputStr()
	{
		mInputString.clear();
	}

	void TextInputProcessor::RenderText(const XMMATRIX * matrix, Color & color)
	{
		XGF_ASSERT(mGetInnerRectangle);
		Shape::Rectangle rc;
		mGetInnerRectangle(rc);
		if (mCaretPos == 0)
		{
			mTextRenderer->DrawStringRtPosition(mInputString.c_str() + mFirstCharPos, color, &rc, matrix, 0);
			mCaretPosition.x = rc.mPolygon.mPoint[0].x + 0.8f;
			mCaretPosition.y = rc.mPolygon.mPoint[0].y + 0.f;
		}
		else if (mCaretPos > 0)
		{
			Position u = mTextRenderer->DrawStringRtPosition(mInputString.c_str() + mFirstCharPos, color, &rc, matrix, mCaretPos);
			mCaretPosition.x = u.x + rc.mPolygon.mPoint[0].x + 0.8f;
			mCaretPosition.y = u.y + rc.mPolygon.mPoint[0].y + 0.f;
		}
		else
		{
			//do nothing
		}
	}

	void TextInputProcessor::BackUp()
	{
		if (mCaretPos > 0)
		{
			mInputString.erase(mCaretPos - 1, 1);
			if (mCaretPos >= 1)
				mCaretPos--;
		}
	}

	void TextInputProcessor::Delete()
	{
		if (mInputString.length() > 0 && mCaretPos <= mInputString.length() - 1)
		{
			mInputString.erase(mCaretPos, 1);
		}
	}

	void TextInputProcessor::CaretToLeft()
	{
		if (mCaretPos > 0)
			mCaretPos--;
	}

	void TextInputProcessor::CaretToRight()
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
	void TextInputProcessor::SetText(const wchar_t * text)
	{
		mInputString = text;
	}
	bool TextInputProcessor::layoutCallBack(int i, wchar_t ch, Position * p, bool c, Position & temp)//c == true is Down
	{
		auto h = mTextRenderer->GetFontSize() + 1;
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
	void TextInputProcessor::CaretToUp()
	{
		XGF_ASSERT(mGetInnerRectangle);
		mCallBackBuffer.first = -1;
		mCallBackBuffer.second = Position(0, 0);
		Position temp;
		Shape::Rectangle rc;
		mGetInnerRectangle(rc);
		temp.x = mCaretPosition.x - rc.mPolygon.mPoint[0].x - 0.8f;
		temp.y = mCaretPosition.y - rc.mPolygon.mPoint[0].y;
		mTextRenderer->GetLayoutShaper().DoLayouShaper(mInputString.c_str() + mFirstCharPos, rc, mTextRenderer->GetFont(), nullptr,
			std::bind(&TextInputProcessor::layoutCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, false, temp));
		if (mCallBackBuffer.first > 0 && mCallBackBuffer.first < mInputString.size())
			mCaretPos = mCallBackBuffer.first;
	}

	void TextInputProcessor::CaretToDowm()
	{
		XGF_ASSERT(mGetInnerRectangle);
		mCallBackBuffer.first = -1;
		mCallBackBuffer.second = Position(0, 0);
		Position temp;
		Shape::Rectangle rc;
		mGetInnerRectangle(rc);
		temp.x = mCaretPosition.x - rc.mPolygon.mPoint[0].x - 0.8f;
		temp.y = mCaretPosition.y - rc.mPolygon.mPoint[0].y;
		mTextRenderer->GetLayoutShaper().DoLayouShaper(mInputString.c_str() + mFirstCharPos, rc, mTextRenderer->GetFont(), nullptr,
			std::bind(&TextInputProcessor::layoutCallBack, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, true, temp));
		if (mCallBackBuffer.first > 0 && mCallBackBuffer.first < mInputString.size())
			mCaretPos = mCallBackBuffer.first;
	}

	void TextInputProcessor::GetCaretProperty(float & x, float & y, int & size)
	{
		x = mCaretPosition.x;
		y = mCaretPosition.y;
		size = mTextRenderer->GetFontSize();
	}

	const wchar_t * TextInputProcessor::GetText()
	{
		return mInputString.c_str();
	}

	void TextInputProcessor::OnFocus(bool isFocus)
	{
		if (mOnFocusListener != nullptr)
			mOnFocusListener(isFocus);
	}

}
