#include "../../Include/TextRenderer.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Polygon.hpp"
#include "../../Include/Rectangle.hpp"
#include "../../Include/Font.hpp"
#include "../../Include/Texture.hpp"
#include "../../Include/ShaderManager.hpp"
#include "../../Include/Context.hpp"

namespace XGF
{
	TextRenderer::TextRenderer() :mTemporarybuffer(nullptr), textureBinder(std::make_shared<PolygonPleTextureBinder>(4)),
		colorBinder(std::make_shared<PolygonPleConstantColorBinder>(4, Color(0.f, 0.f, 0.f, 1.f)))
	{
	}


	TextRenderer::~TextRenderer()
	{
	}

	void TextRenderer::Initialize(Font * font, int MaxCount)
	{
		mFont = font;
		mBatch.Initialize(Context::Current().QueryShaderManager().GetFontShaders(true), MaxCount * 4, MaxCount * 6, TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mBatch.GetShaderStage()->SetBlendState(BlendState::AddZeroOneAdd);
		// 文字渲染采用不同的深度函数方法，在同深度的UI控件绘制中以写入缓冲区
		mBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthEnableWithLessEqual);
		bbridge.AddPlaceHolder();
		bbridge.AddBinder(textureBinder);
		bbridge.AddBinder(colorBinder);
	}

	void TextRenderer::Shutdown()
	{
		mBatch.Shutdown();
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
		DrawString(mTemporarybuffer, SM::Color(1.0f, 1.0f, 1.0f, 1.0f), x, y);
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

	void TextRenderer::DrawString(const wchar_t * str, Color color, const Shape::Rectangle * ppe, const SM::Matrix * matrix)
	{
		colorBinder->Set(0, color);
		auto fun = std::bind(&TextRenderer::AddCharToBatch, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, matrix);
		mLayoutShaper.DoLayouShaper(str, *ppe, *mFont, fun);
	}
	Position TextRenderer::DrawStringRtPosition(const wchar_t * str, Color color, const Shape::Rectangle * ppe, const SM::Matrix * matrix, int pos)
	{
		Position p;
		colorBinder->Set(0, color);
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
		mBatch.GetShaderStage()->SetPSConstantBuffer(0, Color(1.f, 1.f, 1.f, 1.f));
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

	bool TextRenderer::AddCharToBatch(int i, wchar_t ch, Shape::Rectangle * rc, const PosSize *ps, const SM::Matrix * matrix)
	{
		if (matrix != nullptr)
			rc->mPolygon->ExpandAll(Operator::Multiply(*matrix));
		bbridge.SetBinder(rc->mPolygon, 0);// here setBinder
		textureBinder->GetData(1).x = textureBinder->GetData(0).x = ps->metrics.left;
		textureBinder->GetData(2).x = textureBinder->GetData(3).x = ps->metrics.right;
		textureBinder->GetData(1).y = textureBinder->GetData(2).y = ps->metrics.bottom;
		textureBinder->GetData(3).y = textureBinder->GetData(0).y = ps->metrics.top;

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
