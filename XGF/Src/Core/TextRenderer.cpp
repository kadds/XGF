#include "../../Include/TextRenderer.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Log.hpp"
#include "../../Include/Polygon.hpp"
#include "../../Include/Rectangle.hpp"
#include "../../Include/Font.hpp"
#include "../../Include/Texture.hpp"
#include "../../Include/Tools.hpp"
#include "../../Include/ShaderConst.hpp"
TextRenderer::TextRenderer():mTemporarybuffer(nullptr)
{
}


TextRenderer::~TextRenderer()
{
}

void TextRenderer::Initialize(GDI * gdi, Font * font, int MaxCount)
{
	mFont = font;
    InputType inputype[3] = { SHADER_INPUTLAYOUT_POSITION,SHADER_INPUTLAYOUT_COLOR,SHADER_INPUTLAYOUT_TEXTURE };
    mShader.Initialize(gdi, ShaderConst::fontVS, ShaderConst::fontVSSize, ShaderConst::fontPS, ShaderConst::fontPSSize,inputype, 3);
    mBatch.Initialize(gdi, &mShader, MaxCount * 4, MaxCount * 6,RealTime,false);
	mBatch.SetBlend(true);
}

void TextRenderer::Shutdown()
{
    //for each (auto var in map)
    //{
   //     delete var.second;
    //}
    mBatch.Shutdown();
    mShader.Shutdown();
    if (mTemporarybuffer != nullptr)
        delete[] mTemporarybuffer;
}

void TextRenderer::DrawString(const wchar_t * str, int x, int y)
{
    Shape::Rectangle rc;
    rc.SetPositionAndSize(static_cast<float>(x), static_cast<float>(y), 1000.f, 1000.f);
    DrawString(str, -1, -1, Color(1.0f,1.0f,1.0f,1.0f),&rc, nullptr);
}

void TextRenderer::DrawString(const wchar_t * str, Color color, int x, int y)
{
    Shape::Rectangle rc;
    rc.SetPositionAndSize(static_cast<float>(x), static_cast<float>(y), 1000.0f, 1000.0f);
    DrawString(str, -1, -1, color, &rc, nullptr);
}

void TextRenderer::DrawStringEx(int x, int y, const wchar_t * str, ...)
{
    va_list vlArgs = NULL;
    va_start(vlArgs, str);
    size_t nLen = _vscwprintf(str, vlArgs) + 1;
    if(mTemporarybuffer == nullptr)
        mTemporarybuffer = new wchar_t[MAX_TEMPORARY_BUFFER_SIZE];
    _vsnwprintf_s(mTemporarybuffer, nLen, nLen, str, vlArgs);
    va_end(vlArgs);
    DrawString(mTemporarybuffer, Color(1.0f,1.0f,1.0f,1.0f), x, y);
}

void TextRenderer::DrawStringEx(int x, int y, Color color, const wchar_t * str, ...)
{
    va_list vlArgs = NULL;
    va_start(vlArgs, str);
    size_t nLen = _vscwprintf(str, vlArgs) + 1;
    if (mTemporarybuffer == nullptr)
        mTemporarybuffer = new wchar_t[MAX_TEMPORARY_BUFFER_SIZE];
    _vsnwprintf_s(mTemporarybuffer, nLen, nLen, str, vlArgs);
    va_end(vlArgs);
    DrawString(mTemporarybuffer, color,x, y);
}

Position TextRenderer::DrawString(const wchar_t * str, int start, int end, Color color, const Shape::Rectangle * ppe, const XMMATRIX * matrix)
{
    wchar_t c = *str;
    wchar_t lastc = 0;
    int i = 0;
    PosSize *ps;
    int posLeft = 0;
    int posTop = 0;
    int pt = 0;
    Shape::Rectangle py;
    PolygonPleTextureBinder textureBinder(4);
    PolygonPleConstantColorBinder colorBinder(color,4);
	BindingBridge bbridge;
	bbridge.AddBinder(colorBinder);
	bbridge.AddBinder(textureBinder);
	if (start == -1)
		start = 0;
	if (end == -1)
		end = 1000000;
    while (c != L'\0')
    {
		ps = mFont->GetCharGlyph(c);
		pt = mFont->ResizeFontSize(c, lastc);
		if (start <= i && end >= i)
		{
			if (ps == nullptr) 
				return Position(posLeft, posTop);//TODO::ERROR
			textureBinder.SetPosition(ps->left, ps->right, ps->top, ps->bottom);
			if (c == L' ')
				;
			else if (c == L'\n')
			{
				posLeft = -ps->metrics.advanceX;
				posTop += mFont->GetFontSize();
			}
			else
			{
				if (!ppe->SubRectangle(&py, static_cast<float>(posLeft + ps->metrics.xwidth + pt), static_cast<float>(ps->metrics.xheight + posTop),
					static_cast<float>(ps->metrics.width), static_cast<float>(ps->metrics.height)))
				{
					posLeft = 0;
					posTop += mFont->GetFontSize();
					if (!ppe->SubRectangle(&py, static_cast<float>(posLeft + ps->metrics.xwidth + pt), static_cast<float>(ps->metrics.xheight + posTop),
						static_cast<float>(ps->metrics.width), static_cast<float>(ps->metrics.height)))
					{
						posLeft -= pt;
						posLeft += ps->metrics.advanceX;
						lastc = c;
						i++;
						c = *(str + i);
						continue;
					}
				}
				if (matrix != nullptr)
					py.mPolygon.Mul(*matrix);
				py.mPolygon.Transform(static_cast<float>(mBatch.GetClientWidth() / 2), static_cast<float>(mBatch.GetClientHeight() / 2));
				mBatch.DrawPolygon(py.mPolygon, py.GetIndex(), bbridge);
			}
		}
        posLeft -= pt;
        posLeft += ps->metrics.advanceX;
        lastc = c;
		i++;
        c = *(str + i);
        //OutputDebugStringEx("\n");
    }
	return Position(posLeft,posTop);
}

void TextRenderer::Begin(const WVPMatrix & matrix)
{
    //for each (auto var in map)
    //{
    //    for (auto i = var.second->begin(); i != var.second->end(); ++i)
    //    {
    //        i->second = false;
    //    }
    //
    mBatch.Begin(matrix);
	mBatch.SetTexture(mFont->GetShaderResourceView());
}

void TextRenderer::End()
{
    mBatch.End();
}

void TextRenderer::Flush()
{
	mBatch.Flush();
}