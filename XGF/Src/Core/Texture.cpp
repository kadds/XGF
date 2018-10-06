#include "../../Include/Texture.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/DDSTextureLoader.h"
#include "../../Include/WICTextureLoader.h"
#include <algorithm>
#include <fstream>
namespace XGF
{
	Texture::Texture():mIs9Path(false), mTextureResource(nullptr)
	{
		mTextureRectangleNormalization = Point4(0.f, 0.f, 1.f, 1.f);
	}

	Texture::Texture(TextureResource & tres, Rect & rc) :mIs9Path(false)
	{
		if (&tres == nullptr) XGF_Warn(Application, "resource is not defined");
		SetRectangle(rc);
		mTextureResource = &tres;
	}
	Texture::Texture(TextureResource & tres) : mIs9Path(false)
	{
		if (&tres == nullptr) XGF_Warn(Application, "resource is not defined");
		mTextureRectangleNormalization = Point4(0.f, 0.f, 1.f, 1.f);
		mTextureResource = &tres;
	}
	Texture::~Texture()
	{
	}

	void Texture::SetRectangle(Rect & rc)
	{
		mTextureRectangleNormalization.x = rc.Left / static_cast<float>(mTextureResource->mSize.Width);
		mTextureRectangleNormalization.y = rc.Top / static_cast<float>(mTextureResource->mSize.Height);
		mTextureRectangleNormalization.z = rc.Right / static_cast<float>(mTextureResource->mSize.Width);
		mTextureRectangleNormalization.w = rc.Bottom / static_cast<float>(mTextureResource->mSize.Height);

		mTextureRectangleNormalization.x = std::clamp(mTextureRectangleNormalization.x, 0.f, 1.f);
		mTextureRectangleNormalization.y = std::clamp(mTextureRectangleNormalization.y, 0.f, 1.f);
		mTextureRectangleNormalization.z = std::clamp(mTextureRectangleNormalization.z, m9Path.x, 1.f);
		mTextureRectangleNormalization.w = std::clamp(mTextureRectangleNormalization.w, m9Path.y, 1.f);//TODO:: Maybe have problem here
	}
	void Texture::Set9PathInnerRect(Rect & rc)
	{
		m9Path.x = rc.Left / static_cast<float>(mTextureResource->mSize.Width);
		m9Path.y = rc.Top / static_cast<float>(mTextureResource->mSize.Height);
		m9Path.z = rc.Right / static_cast<float>(mTextureResource->mSize.Width);
		m9Path.w = rc.Bottom / static_cast<float>(mTextureResource->mSize.Height);

		m9Path.x = std::clamp(m9Path.x, 0.f, 1.f);
		m9Path.y = std::clamp(m9Path.y, 0.f, 1.f);
		m9Path.z = std::clamp(m9Path.z, m9Path.x, 1.f);
		m9Path.w = std::clamp(m9Path.w, m9Path.y, 1.f);//TODO:: Maybe have problem here

		mIs9Path = true;
	}

	void Texture::Set9PathBorderSize(float border)
	{
		m9Path.x = border / static_cast<float>(mTextureResource->mSize.Width);
		m9Path.y = border / static_cast<float>(mTextureResource->mSize.Height);
		m9Path.z = (mTextureResource->mSize.Width - border) / static_cast<float>(mTextureResource->mSize.Width);
		m9Path.w = (mTextureResource->mSize.Height - border) / static_cast<float>(mTextureResource->mSize.Height);

		m9Path.x = std::clamp(m9Path.x, 0.f, 1.f);
		m9Path.y = std::clamp(m9Path.y, 0.f, 1.f);
		m9Path.z = std::clamp(m9Path.z, m9Path.x, 1.f);
		m9Path.w = std::clamp(m9Path.w, m9Path.y, 1.f);//TODO:: Maybe have problem here
		mIs9Path = true;
	}

	TextureResource::TextureResource():mShaderResourceView(nullptr)
	{

	}

	bool TextureResource::Load(GDI * gdi, void * mem, size_t size)
	{
		if (FAILED(DirectX::CreateDDSTextureFromMemory(gdi->GetDevice(), (uint8_t *)mem, size, (ID3D11Resource **)&mTexture, &mShaderResourceView)))
		{
			if (FAILED(DirectX::CreateWICTextureFromMemory(gdi->GetDevice(), (uint8_t *)mem, size, (ID3D11Resource **)&mTexture, &mShaderResourceView)))
			{
				return false;
			}
		}
		D3D11_TEXTURE2D_DESC desc;
		mTexture->GetDesc(&desc);
		mSize.Height = desc.Height;
		mSize.Width = desc.Width;
		return true;
	}

	bool TextureResource::Load(GDI * gdi, std::wstring fullPath)
	{
		std::ifstream inFile;
		inFile.open(fullPath, std::ios::binary);
		inFile.seekg(0, std::ios::end);  
		size_t length = inFile.tellg();
		inFile.seekg(0, std::ios::beg);
		if (length < (1U << 24) && length > 0)
		{
			char * buffer = new char[length];
			inFile.read(buffer, length);
			bool isok = Load(gdi, buffer, length);
			delete[] buffer;
			inFile.close();
			return isok;
		}
		inFile.close();
		return false;
	}

	void TextureResource::Release()
	{
		if (mShaderResourceView != nullptr)
			mShaderResourceView->Release();
		mShaderResourceView = nullptr;
		if (mTexture != nullptr)
			mTexture->Release();
		mTexture = nullptr;
	}

};
