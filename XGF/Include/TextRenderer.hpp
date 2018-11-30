#pragma once
#include "Defines.hpp"
#include "Rectangle.hpp"
#include "Shader.hpp"
#include "Batch.hpp"
#include "TextLayoutShaper.hpp"
#include <sstream>
namespace XGF
{
	class Font;
	class Batch;
	/*
	ͨ���ı���Ⱦ
	*/
	class TextRenderer
	{
	public:
		TextRenderer();
		~TextRenderer();
		void Initialize(GDI * gdi, Font * font, int MaxCount);
		void Shutdown();
		void DrawString(const wchar_t * str, float x, float y, float z = 0.01f);
		void DrawString(const wchar_t * str, SM::Color color, float x, float y, float z = 0.01f);
		
		template <typename T>
		DEPRECATED("Please use DrawString")
		void DrawStringWithNum(const wchar_t * str, T c, float x, float y)
		{
			std::wstringstream  strk;
			strk.str(str);
			strk << c;
			DrawString(strk.str().c_str(), SM::Color(1.0f, 1.0f, 1.0f, 1.0f), x, y);
		}

		DEPRECATED("Please use DrawString")
		void DrawStringEx(float x, float y, const wchar_t * str, ...);

		DEPRECATED("Please use DrawString")
		void DrawStringEx(float x, float y, SM::Color color, const wchar_t * str, ...);
		//��Ⱦ�ַ��� ��ɫ����Ⱦ�޶����Σ��任����
		void DrawString(const wchar_t * str, SM::Color color, const Shape::Rectangle * ppe, const SM::Matrix * matrix);
		Position DrawStringRtPosition(const wchar_t * str, SM::Color color, const Shape::Rectangle * ppe, const SM::Matrix * matrix, int pos);
		int GetFontSize();
		TextLayoutShaper & GetLayoutShaper() { return mLayoutShaper; }
		Font & GetFont() { return *mFont; };
		void Begin(const WVPMatrix & matrix);
		void End();
		void Flush();

	private:
		bool AddCharToBatch(int i, wchar_t ch, Shape::Rectangle * rc, const PosSize * ps, const SM::Matrix * matrix);
		bool PenMoveCallBackFunction(int i, wchar_t ch, Position * p, int c, Position * v);
		Batch mBatch;
		//�ⲿ
		Font *mFont;
		TextLayoutShaper mLayoutShaper;
		BindingBridge bbridge;
		std::shared_ptr<PolygonPleTextureBinder> textureBinder;
		std::shared_ptr < PolygonPleConstantColorBinder> colorBinder;
		wchar_t * mTemporarybuffer;
	};

	const int MAX_TEMPORARY_BUFFER_SIZE = 512;
};
