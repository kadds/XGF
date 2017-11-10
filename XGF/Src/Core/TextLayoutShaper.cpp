#include "..\..\Include\TextLayoutShaper.hpp"



TextLayoutShaper::TextLayoutShaper()
{
}


TextLayoutShaper::~TextLayoutShaper()
{
}

Position TextLayoutShaper::MeasureStringSize(Font & font, wchar_t * str)
{
	Position pt(0,0);
	return pt;
}

int TextLayoutShaper::DoLayouShaper(const wchar_t * str, const Shape::Rectangle & rc, Font & font, LayoutCallbackFunction fun, PenMoveCallback movec)
{
	PosSize *ps;
	Position pos(0.f, 0.f);
	int i;
	wchar_t c = str[0];
	for (i = 0; c != L'\0' ; i++, c=str[i])
	{
		ps = font.GetCharGlyph(c);
		if (c != L' ')
		{
			if (c == L'\n')
			{
				pos.x = 0.f;
				pos.y += font.GetFontSize() + 1.f;
				if (movec != nullptr)
					if (movec(i, c, &pos))
						return i;
				continue;
			}
			else
			{
				if (!rc.SubRectangle(&mRc, pos.x + ps->vx, pos.y + ps->vy, static_cast<float>(ps->width), static_cast<float>(ps->height)))
				{
					pos.x = 0.f;
					pos.y += font.GetFontSize() + 1.f;
					if (!rc.SubRectangle(&mRc, pos.x + ps->vx, pos.y + ps->vy, static_cast<float>(ps->width), static_cast<float>(ps->height)))
					{
						if (movec != nullptr)
							movec(-1, c, &pos);
						return i;
					}
				}
				if (fun != nullptr && fun(i, c, &mRc, ps))
				{
					pos.x += mRc.mPolygon.mPoint[1].x;
					pos.y += mRc.mPolygon.mPoint[0].y;
				}
				if (movec != nullptr)
					if (movec(i, c, &pos))
						return i;
			}
		}
		
		pos.x += ps->advanceX;
		
	}
	if (movec != nullptr)
		movec(i, L'\0', &pos);
	return i;
}
