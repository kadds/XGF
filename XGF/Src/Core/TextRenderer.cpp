#include "../../Include/TextRenderer.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Log.hpp"
#include "../../Include/Polygon.hpp"
#include "../../Include/Rectangle.hpp"
#include "../../Include/Font.hpp"
#include "../../Include/Texture.hpp"
#include "../../Include/Tools.hpp"
#include "../../Include/ShaderConst.hpp"
#include "../../Include/ConstantData.hpp"
#include "..\..\Include\TextRenderer.hpp"
namespace XGF
{
	TextRenderer::TextRenderer() :mTemporarybuffer(nullptr), textureBinder(4), colorBinder(Color(0.f, 0.f, 0.f, 1.f), 4)
	{
	}


	TextRenderer::~TextRenderer()
	{
	}

	void TextRenderer::Initialize(GDI * gdi, Font * font, int MaxCount)
	{
		mFont = font;
		mBatch.Initialize(gdi, ConstantData::GetInstance().GetFontShaders(), MaxCount * 4, MaxCount * 6, TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mBatch.GetShaderStage()->SetBlendState(BlendState::AddZeroOneAdd);
		mBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthEnable);
		bbridge.AddPlaceHolder();
		bbridge.AddBinder(colorBinder);
		bbridge.AddBinder(textureBinder);
	}

	void TextRenderer::Shutdown()
	{
		mBatch.Shutdown();
		if (mTemporarybuffer != nullptr)
			delete[] mTemporarybuffer;
	}

	void TextRenderer::DrawString(const wchar_t * str, float x, float y, float z)
	{
		Shape::Rectangle rc;
		rc.SetPositionAndSize(x, y, 100000.f, 100000.f);
		rc.SetZ(z);
		DrawString(str, Color(1.0f, 1.0f, 1.0f, 1.0f), &rc, nullptr);
	}

	void TextRenderer::DrawString(const wchar_t * str, Color color, float x, float y, float z)
	{
		Shape::Rectangle rc;
		rc.SetPositionAndSize(static_cast<float>(x), static_cast<float>(y), 1000.0f, 1000.0f);
		rc.SetZ(z);
		DrawString(str, color, &rc, nullptr);
	}

	void TextRenderer::DrawStringEx(float x, float y, const wchar_t * str, ...)
	{
		va_list vlArgs = NULL;
		va_start(vlArgs, str);
		size_t nLen = _vscwprintf(str, vlArgs) + 1;
		if (mTemporarybuffer == nullptr)
			mTemporarybuffer = new wchar_t[MAX_TEMPORARY_BUFFER_SIZE];
		_vsnwprintf_s(mTemporarybuffer, nLen, nLen, str, vlArgs);
		va_end(vlArgs);
		DrawString(mTemporarybuffer, Color(1.0f, 1.0f, 1.0f, 1.0f), x, y);
	}

	void TextRenderer::DrawStringEx(float x, float y, Color color, const wchar_t * str, ...)
	{
		va_list vlArgs = NULL;
		va_start(vlArgs, str);
		size_t nLen = _vscwprintf(str, vlArgs) + 1;
		if (mTemporarybuffer == nullptr)
			mTemporarybuffer = new wchar_t[MAX_TEMPORARY_BUFFER_SIZE];
		_vsnwprintf_s(mTemporarybuffer, nLen, nLen, str, vlArgs);
		va_end(vlArgs);
		DrawString(mTemporarybuffer, color, x, y);
	}

	void TextRenderer::DrawString(const wchar_t * str, Color color, const Shape::Rectangle * ppe, const XMMATRIX * matrix)
	{
		colorBinder.Set(0, 1, color);
		auto fun = std::bind(&TextRenderer::AddCharToBatch, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, matrix);
		mLayoutShaper.DoLayouShaper(str, *ppe, *mFont, fun);
	}
	Position TextRenderer::DrawStringRtPosition(const wchar_t * str, Color color, const Shape::Rectangle * ppe, const XMMATRIX * matrix, int pos)
	{
		Position p;
		colorBinder.Set(0, 1, color);
		auto fun = std::bind(&TextRenderer::AddCharToBatch, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, matrix);
		auto fun2 = std::bind(&TextRenderer::PenMoveCallBackFunction, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, pos, &p);
		mLayoutShaper.DoLayouShaper(str, *ppe, *mFont, fun, fun2);
		return p;
	}
	int TextRenderer::GetFontSize()
	{
		return mFont->GetFontSize();
	}

	void TextRenderer::Begin(const WVPMatrix & matrix)
	{
		mBatch.GetShaderStage()->SetVSConstantBuffer(0, &matrix);
		mBatch.GetShaderStage()->SetPSSRV(0, mFont->GetShaderResourceView());
		mBatch.Begin();
		
	}

	void TextRenderer::End()
	{
		mBatch.End();
	}

	void TextRenderer::Flush()
	{
		mBatch.Flush();
	}

	bool TextRenderer::AddCharToBatch(int i, wchar_t ch, Shape::Rectangle * rc, const PosSize *ps, const XMMATRIX * matrix)
	{
		if (matrix != nullptr)
			rc->mPolygon.Mul(*matrix);
		bbridge.SetBinder(rc->mPolygon, 0);// here setBinder
		textureBinder.SetPosition(ps->metrics.left, ps->metrics.right, ps->metrics.top, ps->metrics.bottom);
		mBatch.DrawPolygon(rc->GetIndex(), bbridge);
		return false;
	}

	bool TextRenderer::PenMoveCallBackFunction(int i, wchar_t ch, Position * p, int c, Position * u)
	{
		if (c == i)
		{
			u->x = p->x;
			u->y = p->y;
		}
		return false;
	}


};
