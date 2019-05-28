#pragma once
#include "Defines.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>
#include <d3d11_1.h>
#include "Texture.hpp"
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
		float advanceX;
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
		int GetFontSize() const { return mFontSize; }
		int ResizeFontSize(wchar_t ch, wchar_t lastch);
		Texture* GetTexture();
		const void* GetFontRawData() const;
		size_t GetFontRawDataSize() const;
		const std::string & GetFontName() const;
	protected:
		FT_Library    pFTLib;
		FT_Face        pFTFace;
		FT_GlyphSlot            mFTSlot;

		int mFontSize;
		float       mAddance;
		float           mToplineHeigh;
		std::unique_ptr<char> mFileBuffer;
		size_t mFileSize;
	private:
		void BindRenderer();
		long ReadFileToBuffer(const std::wstring & name);
		void CloseFileBuffer();
		std::unordered_map<wchar_t, PosSize*> map;
		unsigned            mBufferWidth;
		unsigned            mBufferHeight;
		std::unique_ptr<Texture> mTexture;
		std::unique_ptr<char[]> mBuffer;
		Rectangle mDirtyRectangle;
		bool mDirty;
		std::string mName;
	};


}

