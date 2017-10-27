#pragma once
#include "Defines.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include <d3d11_1.h>
#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib,"../Other/freetype/libs/dll/x64/freetype28MTD.lib")
#else
#pragma comment(lib,"../Other/freetype/libs/dll/Win32/freetype28MTD.lib")
#endif
#else
#ifdef _WIN64
#pragma comment(lib,"../Other/freetype/libs/dll/x64/freetype28MT.lib")
#else
#pragma comment(lib,"../Other/freetype/libs/dll/Win32/freetype28MT.lib")
#endif
#endif

#include <iostream>  
#include <fstream>  
#include <unordered_map>


struct FontMetrics {
	int height;
	int width;
};
class GDI;
struct PosSize
{
	float left;
	float right;
	float top;
	float bottom;
	int advanceX;//��������
	int n;
	int vx;
	int vy;
	FontMetrics metrics;
};
class Font
{
public:
	Font();
	~Font();
	bool Initialize(GDI * gdi, const char * name, int fontSize);
	void Shutdown();
	PosSize* GetCharGlyph(wchar_t ch);
	ID3D11ShaderResourceView * GetShaderResourceView() { return mShaderResourceView; }
	int GetFontSize() { return mFontSize; }
	int ResizeFontSize(wchar_t ch, wchar_t lastch) ;
protected:
	FT_Library    pFTLib;
	FT_Face        pFTFace;
	FT_GlyphSlot            mFTSlot;

	int mFontSize;
	float       mAddance;
	float           mToplineHeigh;
	char * mFileBuffer;
	long mFileLen;
private:
    void CreateView();
	void ReadFileToBuffer(const char * name);
	void CloseFileBuffer();
	std::unordered_map<wchar_t, PosSize*> map;
	byte * mBuffer;
	unsigned            mBufferWidth;
	unsigned            mBufferHeight;
	ID3D11ShaderResourceView * mShaderResourceView;
    ID3D11Texture2D* mTexture;
	GDI* mGDI;
};
