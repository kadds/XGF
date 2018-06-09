#pragma once
#include "Defines.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include <d3d11_1.h>
#ifdef XGF_USE_FREETYPE_STATIC
#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib,"./../../External/freetype/libs/lib/x64/Debug/freetyped.lib")
#else
#pragma comment(lib,"./../../External/freetype/libs/lib/Win32/Debug/freetyped.lib")
#endif
#else
#ifdef _WIN64
#pragma comment(lib,"./../../External/freetype/libs/lib/x64/Release/freetype.lib")
#else
#pragma comment(lib,"./../../External/freetype/libs/lib/Win32/Release/freetype.lib")
#endif
#endif
#else //自行编译
#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib,"./../../External/freetype/libs/dll/x64/Debug/freetyped.lib")
#else
#pragma comment(lib,"./../../External/freetype/libs/dll/Win32/Debug/freetyped.lib")
#endif
#else
#ifdef _WIN64
#pragma comment(lib,"./../../External/freetype/libs/dll/x64/Release/freetype.lib")
#else
#pragma comment(lib,"./../../External/freetype/libs/dll/Win32/Release/freetype.lib")
#endif
#endif
#endif
#include <iostream>  
#include <fstream>  
#include <unordered_map>
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
		float advanceX;//步进距离
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
		bool Initialize(GDI * gdi, const std::wstring & name, int fontSize);
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
		char * mFileBuffer;
	private:
		void CreateView();
		long ReadFileToBuffer(const std::wstring & name);
		void CloseFileBuffer();
		std::unordered_map<wchar_t, PosSize*> map;
		byte * mBuffer;
		unsigned            mBufferWidth;
		unsigned            mBufferHeight;
		ID3D11ShaderResourceView * mShaderResourceView;
		ID3D11Texture2D* mTexture;
		GDI* mGDI;
	};


}

