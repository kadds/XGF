#include "../../Include/Texture.hpp"
#include "../../Include/Logger.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/DDSTextureLoader.h"
#include "../../Include/WICTextureLoader.h"
#include <fstream>
#include "../../Include/Context.hpp"
#include "../../Include/Shader.hpp"
#include "../../Include/ScreenGrab.h"
#include <wincodec.h>
#include "../../Include/Renderer.hpp"

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

	void Texture::GetSize(int& width, int& height)
	{
		if (GetTextureResource().GetTexture2D())
		{
			D3D11_TEXTURE2D_DESC desc;
			GetTextureResource().GetTexture2D()->GetDesc(&desc);
			width = desc.Width;
			height = desc.Height;
		}
	}

	TextureFormat Texture::GetFormat()
	{
		if (GetTextureResource().GetTexture2D())
		{
			D3D11_TEXTURE2D_DESC desc;
			GetTextureResource().GetTexture2D()->GetDesc(&desc);
			return desc.Format;
		}
		return TextureFormat::DXGI_FORMAT_UNKNOWN;
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
		return false;
	}

	Texture::~Texture()
	{
	}

	TextureResource& Texture::GetTextureResource()
	{
		return mTextureResource;
	}

	void Texture::UpdateTexture(char * ptr, const Rectangle & rect)
	{
		Context::Current().QueryRenderer().AppendBeforeDrawCallbackOnce([rect, ptr, this](Renderer* r)
			{
				auto* dc = Context::Current().QueryGraphicsDeviceInterface().GetDeviceContext();
				if (rect.width <= 0 || rect.height <= 0) return;
				D3D11_BOX box;
				box.left = rect.x;
				box.right = rect.x + rect.width;
				box.top = rect.y;
				box.bottom = rect.y + rect.height;
				box.front = 0;
				box.back = 1;
				int width, height;
				GetSize(width, height);
				// here update dirty rectangle
				dc->UpdateSubresource(GetRawTexture2D(), 0, &box, ptr, width, 0);
			});
		
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

	bool Texture::Load(void * mem, size_t size)
	{
		auto& gdi = Context::Current().QueryGraphicsDeviceInterface();
		if (FAILED(DirectX::CreateDDSTextureFromMemory(gdi.GetDevice(), (uint8_t *)mem, size, (ID3D11Resource **)&GetTextureResource().texture2d, &GetTextureResource().shaderResourceView)))
		{
			if (FAILED(DirectX::CreateWICTextureFromMemory(gdi.GetDevice(), (uint8_t *)mem, size, (ID3D11Resource **)& GetTextureResource().texture2d, &GetTextureResource().shaderResourceView)))
			{
				return false;
			}
		}
		mHasLoad = true;
		return true;
	}

	bool Texture::Load(std::wstring fullPath)
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

	bool Texture::Load(ID3D11ShaderResourceView* srv, ID3D11Texture2D* texture2D)
	{
		GetTextureResource().shaderResourceView = srv;
		GetTextureResource().texture2d = texture2D;
		mHasLoad = true;
		return true;
	}

	void Texture::Create(unsigned width, unsigned height, TextureFormat format, char* ptr, int pitch, int slicePitch)
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		auto srv = gdi.CreateRenderableTexture(width, height, format, ptr, pitch, slicePitch);
		ID3D11Resource * resource;
		srv->GetResource(&resource);
		GetTextureResource().shaderResourceView = srv;
		GetTextureResource().texture2d = static_cast<ID3D11Texture2D*>(resource);
		mHasLoad = true;
	}

	bool Texture::HasLoad() const
	{
		return mHasLoad;
	}

};
