#include "../../Include/Texture.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/DDSTextureLoader.h"
#include "../../Include/WICTextureLoader.h"
#include <algorithm>
#include <fstream>
#include "../../Include/Context.hpp"
#include "../../Include/Shader.hpp"
#include "../../Include/ScreenGrab.h"
#include <wincodec.h>

namespace XGF
{
	Texture::Texture()
	{
	}

	ID3D11ShaderResourceView* Texture::GetRawTexture() const
	{
		return mTextureResource.GetSRV();
	}

	ID3D11Texture2D* Texture::GetRawTexture2D() const
	{
		return mTextureResource.GetTexture2D();
	}

	unsigned Texture::GetWidth() const
	{
		return mTextureResource.GetWidth();
	}

	unsigned Texture::GetHeight() const
	{
		return mTextureResource.GetHeight();
	}

	bool Texture::SaveAs(ImageType type, const std::string& path)
	{
		GUID format;
		switch (type) {
		case ImageType::DDS:
			{
				auto hr = DirectX::SaveDDSTextureToFile(Context::Current().QueryGraphicsDeviceInterface().GetDeviceContext(),
					mTextureResource.GetTexture2D(), fmt::to_wstring(path).c_str());
				XGF_Info_Check(Framework, hr, ". Save texture to disk failed");
				if (FAILED(hr))
					return false;
				else
				{
					XGF_Info(Framework, "Save texture to disk successfully");
					return true;
				}
			}
			break;
		case ImageType::PNG:
			format = GUID_ContainerFormatPng;
			break;
		case ImageType::JPEG:
			format = GUID_ContainerFormatJpeg;
		case ImageType::BMP:
			format = GUID_ContainerFormatBmp;
			break;
		default:;
		}
		if(type != ImageType::DDS)
		{
			auto hr = DirectX::SaveWICTextureToFile(Context::Current().QueryGraphicsDeviceInterface().GetDeviceContext(),
				mTextureResource.GetTexture2D(), format, fmt::to_wstring(path).c_str());
			XGF_Info_Check(Framework, hr, ". Save texture to disk failed");
			if (FAILED(hr))
				return false;
			else
			{
				XGF_Info(Framework, "Save texture to disk successfully");
				return true;
			}
		}
	}

	Texture::~Texture()
	{
	}

	TextureResource& Texture::GetTextureResource()
	{
		return mTextureResource;
	}

	DynamicTexture::DynamicTexture(unsigned width, unsigned height, TextureFormat format, char * ptr): mPtr(ptr)
	{
		GetTextureResource().Create(width, height, format, ptr);
	}

	void DynamicTexture::UpdateDirtyRectangle(const Rectangle & rect)
	{
		mDirtyRectangle = Rectangle::Union(rect, mDirtyRectangle);
	}

	void DynamicTexture::ClearDirtyRectangle()
	{
		mDirtyRectangle.width = 0;
		mDirtyRectangle.height = 0;
		mDirtyRectangle.x = 0;
		mDirtyRectangle.y = 0;
	}

	const Rectangle& DynamicTexture::GetDirtyRectangle() const
	{
		return mDirtyRectangle;
	}

	void DynamicTexture::UpdateTexture()
	{
		auto * dc = Context::Current().QueryGraphicsDeviceInterface().GetDeviceContext();
		auto & rect = GetDirtyRectangle();
		if (rect.width <= 0 || rect.height <= 0) return;
		D3D11_BOX box;
		box.left = rect.x;
		box.right = rect.x + rect.width;
		box.top = rect.y;
		box.bottom = rect.y + rect.height;
		box.front = 0;
		box.back = 1;
		// here update dirty rectangle
		dc->UpdateSubresource(GetRawTexture2D(), 0, &box, mPtr, GetWidth(), 0);
		ClearDirtyRectangle();
	}


	TextureResource::TextureResource() :shaderResourceView(nullptr), texture2d(nullptr)
	{

	}

	TextureResource::~TextureResource()
	{
		if (shaderResourceView)
			shaderResourceView->Release();
		shaderResourceView = nullptr;
		if (texture2d)
			texture2d->Release();
		texture2d = nullptr;
	}

	bool TextureResource::Load(void * mem, size_t size)
	{
		auto& gdi = Context::Current().QueryGraphicsDeviceInterface();
		if (FAILED(DirectX::CreateDDSTextureFromMemory(gdi.GetDevice(), (uint8_t *)mem, size, (ID3D11Resource **)&texture2d, &shaderResourceView)))
		{
			if (FAILED(DirectX::CreateWICTextureFromMemory(gdi.GetDevice(), (uint8_t *)mem, size, (ID3D11Resource **)&texture2d, &shaderResourceView)))
			{
				return false;
			}
		}
		D3D11_TEXTURE2D_DESC desc;
		texture2d->GetDesc(&desc);
		width = desc.Width;
		height = desc.Height;
		return true;
	}

	bool TextureResource::Load(std::wstring fullPath)
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
			bool isok = Load(buffer, length);
			delete[] buffer;
			inFile.close();
			return isok;
		}
		inFile.close();
		return false;
	}

	void TextureResource::Create(unsigned width, unsigned height, TextureFormat format, char* ptr)
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		auto srv = gdi.CreateRenderableTexture(width, height, format, ptr);
		ID3D11Resource * resource;
		srv->GetResource(&resource);
		shaderResourceView = srv;
		texture2d = static_cast<ID3D11Texture2D*>(resource);
		this->width = width;
		this->height = height;
	}

	void TextureResource::SetTexture2D(ID3D11Texture2D* t2d)
	{
		if(texture2d)
		{
			texture2d->Release();
		}
		texture2d = t2d;
	}

	void TextureResource::SetSRV(ID3D11ShaderResourceView* srv)
	{
		if(shaderResourceView)
		{
			shaderResourceView->Release();
		}
		shaderResourceView = srv;
	}
};
