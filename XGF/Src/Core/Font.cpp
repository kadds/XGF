#include "../../Include/Font.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Tools.hpp"
#include <fstream>  
#include "../../Include/Context.hpp"

namespace XGF
{
	Font::Font() :mFileBuffer(nullptr)
	{
	}


	Font::~Font()
	{
	}

	bool Font::Initialize(const std::wstring & name, int fontSize)
	{
		FT_Error    error = 0;
		error = FT_Init_FreeType(&pFTLib);
		if (error)
		{
			pFTLib = 0;
			return false;
		}
		mFontSize = fontSize;
		mBufferWidth = 256;//TODO:: Change
		mBufferHeight = 256;
		//error = FT_New_Memory_Face(pFTLib,(FT_Byte*) mFileBuffer, mFileLen, 0, &pFTFace);

		mBuffer = std::unique_ptr<char[]>(new char[mBufferWidth*mBufferHeight]);
		memset(mBuffer.get(), 0, sizeof(char)*mBufferWidth*mBufferHeight);
		error = FT_New_Face(pFTLib, Tools::WcharToChar(name.c_str(), name.length() + 1).c_str(), 0, &pFTFace);
		mTexture = std::make_unique<DynamicTexture>(mBufferWidth, mBufferHeight, DXGI_FORMAT_A8_UNORM, mBuffer.get());
		if (!error)
		{
			FT_Select_Charmap(pFTFace, FT_ENCODING_UNICODE);
			FT_Set_Pixel_Sizes(pFTFace, fontSize, fontSize);
			return true;
		}
		else
		{
			long size = ReadFileToBuffer(name);
			error = FT_New_Memory_Face(pFTLib, reinterpret_cast<unsigned char *>(mFileBuffer.get()), size, 0, &pFTFace);
			if (!error)
			{
				FT_Select_Charmap(pFTFace, FT_ENCODING_UNICODE);
				FT_Set_Pixel_Sizes(pFTFace, fontSize, fontSize);
				return true;
			}
			XGF_Error(Application, "File can't open!", Tools::WcharToChar(name.c_str(), name.length() + 1).c_str());
			return false;
		}
	}
	
	long Font::ReadFileToBuffer(const std::wstring& name)
	{
		auto e = Tools::LoadFromFile(name);
		mFileBuffer = std::move(e.first);
		return e.second;
	}

	void Font::Shutdown()
	{
		if (pFTFace)
			FT_Done_Face(pFTFace);
		if (pFTLib)
			FT_Done_FreeType(pFTLib);
		pFTLib = nullptr;
		pFTFace = nullptr;
		mTexture.reset();
		mBuffer.reset();
		CloseFileBuffer();
		auto bg = map.begin();
		for (auto it = map.begin(); it != map.end(); ++it)
		{
			delete it->second;
		}
	}

	PosSize * Font::GetCharGlyph(wchar_t ch) {
		if (pFTFace == nullptr) return nullptr;
		auto cg = map.find(ch);
		if (cg != map.end())
		{
			return cg->second;
		}
		int ascender = pFTFace->size->metrics.ascender >> 6;
		FT_Glyph glyph;
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

		auto device = gdi.GetDevice();
		FT_Load_Glyph(pFTFace, FT_Get_Char_Index(pFTFace, ch), FT_LOAD_DEFAULT);
		FT_Get_Glyph(pFTFace->glyph, &glyph);
		FT_Render_Glyph(pFTFace->glyph, FT_RENDER_MODE_NORMAL);
		FT_Bitmap  &bitmap = pFTFace->glyph->bitmap;
		FT_GlyphSlot slot = pFTFace->glyph;
		int width = bitmap.width;
		int height = bitmap.rows;
		PosSize  *result = new PosSize();
		unsigned int size = static_cast<unsigned int>(map.size());
		result->n = size;
		unsigned left = ((result->n) % (mBufferWidth / (mFontSize + 1))) * mFontSize + 1;
		unsigned top = (result->n / (mBufferHeight / (mFontSize + 1)))* mFontSize + 1;
		unsigned bottom = top + height;
		unsigned right = left + width;
		if (bottom > mBufferHeight || right > mBufferWidth)
			return nullptr;//TODO::Error!
		for (unsigned i = 0; i < bitmap.rows; ++i)
		{
			for (unsigned j = 0; j < bitmap.width; ++j)
			{
				mBuffer[left + (i + top)*mBufferWidth + j] = bitmap.buffer[i * bitmap.width + j];
			}
		}
		result->metrics.left = left / static_cast<float>(mBufferWidth);
		result->metrics.right = right / static_cast<float>(mBufferWidth);
		result->metrics.top = top / static_cast<float>(mBufferHeight);
		result->metrics.bottom = bottom / static_cast<float>(mBufferHeight);
		result->width = slot->metrics.width / 64.0f;
		result->height = slot->metrics.height / 64.0f;
		result->advanceX = slot->advance.x / 64.0f;
		result->vx = static_cast<float>(slot->bitmap_left);
		result->vy = static_cast<float>(-slot->bitmap_top + ascender);
		map.insert(std::pair<wchar_t, PosSize  *>(ch, result));
		FT_Done_Glyph(glyph);
		mTexture->UpdateDirtyRectangle(Rectangle(left, top, bitmap.width, bitmap.rows));
		return result;
	}

	int Font::ResizeFontSize(wchar_t ch, wchar_t lastch)
	{
		FT_Vector vet;
		FT_Get_Kerning(pFTFace, lastch, ch, FT_Kerning_Mode::FT_KERNING_DEFAULT, &vet);

		return vet.x >> 6;
	}

	Texture* Font::GetTexture()
	{
		return mTexture.get();
	}

	void Font::CloseFileBuffer()
	{
		mFileBuffer.reset(nullptr);
	}

}
