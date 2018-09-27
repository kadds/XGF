#include "../../Include/Font.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Tools.hpp"
namespace XGF
{
	Font::Font() :mShaderResourceView(nullptr), mFileBuffer(nullptr)
	{
	}


	Font::~Font()
	{
	}

	bool Font::Initialize(GDI * gdi, const std::wstring & name, int fontSize)
	{
		mGDI = gdi;
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

		mBuffer = new byte[mBufferWidth*mBufferHeight];
		memset(mBuffer, 0, sizeof(byte)*mBufferWidth*mBufferHeight);
		error = FT_New_Face(pFTLib, Tools::WcharToChar(name.c_str(), name.length() + 1).c_str(), 0, &pFTFace);
		CreateView();
		if (!error)
		{
			FT_Select_Charmap(pFTFace, FT_ENCODING_UNICODE);
			FT_Set_Pixel_Sizes(pFTFace, fontSize, fontSize);
			return true;
		}
		else
		{
			long size = ReadFileToBuffer(name);
			error = FT_New_Memory_Face(pFTLib, (unsigned char *) mFileBuffer, size, 0, &pFTFace);
			if (!error)
			{
				FT_Select_Charmap(pFTFace, FT_ENCODING_UNICODE);
				FT_Set_Pixel_Sizes(pFTFace, fontSize, fontSize);
				return true;
			}
			XGF_Error(Application, "File can't open!", name);
			return false;
		}
	}
	void Font::CreateView()
	{
		D3D11_SUBRESOURCE_DATA __subData;
		__subData.pSysMem = mBuffer;
		__subData.SysMemPitch = mBufferWidth;
		__subData.SysMemSlicePitch = mBufferWidth*mBufferHeight;
		D3D11_TEXTURE2D_DESC  Tex2Dtdesc;

		Tex2Dtdesc.Width = mBufferWidth;
		Tex2Dtdesc.Height = mBufferHeight;
		Tex2Dtdesc.MipLevels = 1;
		Tex2Dtdesc.ArraySize = 1;

		Tex2Dtdesc.SampleDesc.Count = 1;
		Tex2Dtdesc.SampleDesc.Quality = 0;
		Tex2Dtdesc.Usage = D3D11_USAGE_DEFAULT;
		Tex2Dtdesc.Format = DXGI_FORMAT_A8_UNORM;
		Tex2Dtdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		Tex2Dtdesc.CPUAccessFlags = 0;
		Tex2Dtdesc.MiscFlags = 0;

		XGF_Error_Check(Application, mGDI->GetDevice()->CreateTexture2D(&Tex2Dtdesc, &__subData, &mTexture), "CreateTexture2D at font class failed");

		PutDebugString(mTexture);
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = Tex2Dtdesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipLevels = Tex2Dtdesc.MipLevels;
		viewDesc.Texture2D.MostDetailedMip = 0;

		XGF_Error_Check(Application, mGDI->GetDevice()->CreateShaderResourceView(mTexture, &viewDesc, &mShaderResourceView), "Create font SRV failed");
		PutDebugString(mShaderResourceView);

	}
	void Font::Shutdown()
	{
		if (pFTFace)
			FT_Done_Face(pFTFace);
		if (pFTLib)
			FT_Done_FreeType(pFTLib);
		pFTLib = nullptr;
		pFTFace = nullptr;
		mTexture->Release();
		if (mShaderResourceView != nullptr)
			mShaderResourceView->Release();
		delete[] mBuffer;
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
		auto device = ((GDI *)(mGDI))->GetDevice();
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
		mGDI->GetDeviceContext()->UpdateSubresource(mTexture, 0, NULL, mBuffer, mBufferWidth, 0);

		return result;
	}

	int Font::ResizeFontSize(wchar_t ch, wchar_t lastch)
	{
		FT_Vector vet;
		FT_Get_Kerning(pFTFace, lastch, ch, FT_Kerning_Mode::FT_KERNING_DEFAULT, &vet);

		return vet.x >> 6;
	}

	long Font::ReadFileToBuffer(const std::wstring & name)
	{
		std::filebuf *pbuf;
		std::ifstream filestr;
		long size;
		filestr.open(name, std::ios::binary);
		pbuf = filestr.rdbuf();

		size = static_cast<long>(pbuf->pubseekoff(0, std::ios::end, std::ios::in));
		pbuf->pubseekpos(0, std::ios::in);
		if (size > 0)
		{
			mFileBuffer = new char[size];
			pbuf->sgetn(mFileBuffer, size);
		}
		else
		{
			XGF_Error(Application, "File not exist!", name);
		}
		filestr.close();
		return size;
	}

	void Font::CloseFileBuffer()
	{
		if (mFileBuffer)
			delete[] mFileBuffer;
		mFileBuffer = nullptr;
	}

}
