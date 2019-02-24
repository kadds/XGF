#pragma once
#include "Defines.hpp"
#include "Rectangle.hpp"
#include "Shader.hpp"
#include "Polygon.hpp"
#include "TextLayoutShaper.hpp"

namespace XGF
{
	class Font;
	class Batch;
	/*
	Universal text renderer
	*/
	class TextRenderer
	{
	public:
		TextRenderer();
		~TextRenderer();
		void Initialize(Font * font, int countPerBatch);
		void Shutdown();
		void DrawString(const string & str, float x, float y, float z = 0.01f);
		void DrawString(const string & str, Color color, float x, float y, float z = 0.01f);
		
		void DrawString(const string & str, Color color, const Shape::Rectangle * ppe, const SM::Matrix * matrix);
		Position DrawStringRtPosition(const string& str, Color color, const Shape::Rectangle* ppe,
		                              const SM::Matrix* matrix,
		                              int pos);
		int GetFontSize();
		TextLayoutShaper & GetLayoutShaper() { return mLayoutShaper; }
		Font & GetFont() const { return *mFont; };
		void Begin(const WVPMatrix & matrix);
		void End();
		void Flush();

	private:
		bool AddCharToBatch(int i, wchar_t ch, Shape::Rectangle * rc, const PosSize * ps, const SM::Matrix * matrix);
		bool PenMoveCallBackFunction(int i, wchar_t ch, Position * p, int c, Position * v);
		RenderState mRenderState;
		RenderResource mRenderResource;
		
		Font *mFont;
		TextLayoutShaper mLayoutShaper;
		BindingBridge bbridge;
		std::shared_ptr<PolygonPleTextureBinder> textureBinder;
		std::shared_ptr<PolygonPleConstantColorBinder> colorBinder;
		std::shared_ptr<PolygonPlePointBinder> pointBinder;
		std::shared_ptr<PolygonPleIndex> indexData;
		int mCurrentPos;
		int mMaxBatchCount;
	};
};
