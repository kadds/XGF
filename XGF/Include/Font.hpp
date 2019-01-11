#pragma once
#include "Defines.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>
#include <d3d11_1.h>

namespace XGF
{
	struct FontMetrics {

		float left;
		float right;
		float top;
		float bottom;
	};
	class GDI;
	struct PosSize
	{
		float height;
		float width;
		float advanceX;//��������
		int n;
		float vx;
		float vy;
		FontMetrics metrics;
	};
	class Font
	{
	public:
		Font();
		~Font();
		bool Initialize(const std::wstring & name, int fontSize);
		void Shutdown();
		PosSize* GetCharGlyph(wchar_t ch);
		ID3D11ShaderResourceView * GetShaderResourceView() { return mShaderResourceView; }
		int GetFontSize() { return mFontSize; }
		int ResizeFontSize(wchar_t ch, wchar_t lastch);
	protected:
		FT_Library    pFTLib;
		FT_Face        pFTFace;
		FT_GlyphSlot            mFTSlot;

		int mFontSize;
		float       mAddance;
		float           mToplineHeigh;
		std::unique_ptr<char> mFileBuffer;
	private:
		void CreateView();
		long ReadFileToBuffer(const std::wstring & name);
		void CloseFileBuffer();
		std::unordered_map<wchar_t, PosSize*> map;
		unsigned char * mBuffer;
		unsigned            mBufferWidth;
		unsigned            mBufferHeight;
		ID3D11ShaderResourceView * mShaderResourceView;
		ID3D11Texture2D* mTexture;
	};


}

