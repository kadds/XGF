#include "../../Include/TextRenderer.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Polygon.hpp"
#include "../../Include/Rectangle.hpp"
#include "../../Include/Font.hpp"
#include "../../Include/Texture.hpp"
#include "../../Include/ShaderManager.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/Renderer.hpp"

namespace XGF
{
	TextRenderer::TextRenderer()
		
	{
	}


	TextRenderer::~TextRenderer()
	{
	}

	void TextRenderer::Initialize(Font * font, int countPerBatch)
	{
		mFont = font;
		int vertex = countPerBatch * 4;
		pointBinder = std::make_shared<PolygonPlePointBinder>(vertex);
		colorBinder = std::make_shared<PolygonPleConstantColorBinder>(vertex, 4);
		textureBinder = std::make_shared<PolygonPleTextureBinder>(vertex);
		int index = countPerBatch * 6;
		indexData = std::make_shared<PolygonPleIndex>(index);
		for(int i = 0; i < index; )
		{
			indexData->Get(i++) = 0;
			indexData->Get(i++) = 3;
			indexData->Get(i++) = 2;
			indexData->Get(i++) = 2;
			indexData->Get(i++) = 1;
			indexData->Get(i++) = 0;
		}
		mShaderStage.Initialize(Context::Current().QueryShaderManager().GetFontShaders(true));
		mShaderStage.SetBlendState(BlendState::AddZeroOneAdd);
		// 文字渲染采用不同的深度函数方法，在同深度的UI控件绘制中以写入缓冲区
		mShaderStage.SetDepthStencilState(DepthStencilState::DepthEnableWithLessEqual);
		bbridge.AddBinder(pointBinder);
		bbridge.AddBinder(textureBinder);
		bbridge.AddBinder(colorBinder);
		mCurrentPos = 0;
		mMaxBatchCount = countPerBatch;
	}

	void TextRenderer::Shutdown()
	{
	}

	void TextRenderer::DrawString(const string & str, float x, float y, float z)
	{
		Shape::Rectangle rc;
		rc.SetPositionAndSize(x, y, 100000.f, 100000.f);
		rc.SetZ(z);
		DrawString(str, Color(1.0f, 1.0f, 1.0f, 1.0f), &rc, nullptr);
	}

	void TextRenderer::DrawString(const string & str, Color color, float x, float y, float z)
	{
		Shape::Rectangle rc;
		rc.SetPositionAndSize(static_cast<float>(x), static_cast<float>(y), 1000.0f, 1000.0f);
		rc.SetZ(z);
		DrawString(str, color, &rc, nullptr);
	}

	void TextRenderer::DrawString(const string & str, Color color, const Shape::Rectangle * ppe, const SM::Matrix * matrix)
	{
		int count = str.size();
		int lastStart = 0;
		int restCount;
		while (count > 0)
		{
			restCount = min(count, mMaxBatchCount - mCurrentPos);
			colorBinder->Set(mCurrentPos, restCount, color);
			auto fun = std::bind(&TextRenderer::AddCharToBatch, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, matrix);
			mLayoutShaper.DoLayouShaper(str.substr(lastStart, restCount), *ppe, *mFont, fun);
			lastStart += restCount;
			count -= restCount;
		}
	}
	Position TextRenderer::DrawStringRtPosition(const string & str, Color color, const Shape::Rectangle * ppe, const SM::Matrix * matrix, int pos)
	{
		int count = str.size();
		int lastStart = 0;
		int restCount = mMaxBatchCount - mCurrentPos;
		Position p;
		while (count > 0)
		{
			restCount = min(count, mMaxBatchCount - mCurrentPos);
			colorBinder->Set(mCurrentPos, restCount, color);
			auto fun = std::bind(&TextRenderer::AddCharToBatch, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, matrix);
			auto fun2 = std::bind(&TextRenderer::PenMoveCallBackFunction, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, pos, &p);

			mLayoutShaper.DoLayouShaper(str.substr(lastStart, restCount), *ppe, *mFont, fun, fun2);
			lastStart += restCount;
			count -= restCount;
		}
		return p;
	}
	int TextRenderer::GetFontSize()
	{
		return mFont->GetFontSize();
	}

	void TextRenderer::Begin(const WVPMatrix & matrix)
	{
		mShaderStage.SetVSConstantBuffer(0, &matrix);
		mShaderStage.SetPSConstantBuffer(0, Color(1.f, 1.f, 1.f, 1.f));
		mShaderStage.SetPSTexture(0, mFont->GetTexture());		
	}

	void TextRenderer::End()
	{
		Flush();
	}

	void TextRenderer::Flush()
	{
		if (mCurrentPos == 0) return;
		indexData->SetActualCount(mCurrentPos * 6);
		pointBinder->SetActualCount(mCurrentPos * 4);
		colorBinder->SetActualCount(mCurrentPos * 4);
		textureBinder->SetActualCount(mCurrentPos * 4);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(
			bbridge, *indexData.get(), mShaderStage
		));
		mCurrentPos = 0;
	}

	bool TextRenderer::AddCharToBatch(int i, wchar_t ch, Shape::Rectangle * rc, const PosSize *ps, const SM::Matrix * matrix)
	{
		if (matrix != nullptr)
			rc->mPolygon->ExpandAll(Operator::Multiply(*matrix));
		const int start = mCurrentPos * 4;
		int istart = mCurrentPos * 6;
		textureBinder->GetData(1 + start).x = textureBinder->GetData(0 + start).x = ps->metrics.left;
		textureBinder->GetData(2 + start).x = textureBinder->GetData(3 + start).x = ps->metrics.right;
		textureBinder->GetData(1 + start).y = textureBinder->GetData(2 + start).y = ps->metrics.bottom;
		textureBinder->GetData(3 + start).y = textureBinder->GetData(0 + start).y = ps->metrics.top;

		auto rawPtr = pointBinder->GetData() + start;
		rc->mPolygon->CopyTo(rawPtr, sizeof(Point));

		indexData->Get(istart++) = 0 + start;
		indexData->Get(istart++) = 3 + start;
		indexData->Get(istart++) = 2 + start;
		indexData->Get(istart++) = 2 + start;
		indexData->Get(istart++) = 1 + start;
		indexData->Get(istart++) = 0 + start;

		mCurrentPos++;
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
