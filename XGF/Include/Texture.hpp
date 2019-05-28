#pragma once
#include "Defines.hpp"
#define NOMINMAX
#include <d3d11_1.h>
namespace XGF
{

	class GDI;
	class Renderer;
	typedef DXGI_FORMAT TextureFormat;
	struct TextureResource
	{
	private:
		ID3D11ShaderResourceView * shaderResourceView;
		ID3D11Texture2D * texture2d;
	public:
		TextureResource();
		~TextureResource();

		ID3D11Texture2D * GetTexture2D() const
		{
			return texture2d;
		}
		ID3D11ShaderResourceView * GetSRV() const
		{
			return shaderResourceView;
		}
		friend class Texture;
	};

	class Texture
	{
	public:
		friend class Renderer;
		Texture();
		virtual ~Texture();
		TextureResource& GetTextureResource();
		ID3D11ShaderResourceView* GetRawTexture() const;
		ID3D11Texture2D * GetRawTexture2D() const;
		void GetSize(int& width, int& height);
		TextureFormat GetFormat();
		void UpdateTexture(char *ptr, const Rectangle& rect);
		enum class ImageType
		{
			DDS,
			PNG,
			JPEG,
			BMP,
		};
		bool SaveAs(ImageType type, const std::string& path);
		bool Load(void* mem, size_t size);
		bool Load(std::wstring fullPath);
		bool Load(ID3D11ShaderResourceView* srv, ID3D11Texture2D* texture2D);
		void Create(unsigned width, unsigned height, TextureFormat format, char* ptr, int pitch, int slicePitch = 0);
		bool HasLoad() const;
	private:
		TextureResource mTextureResource;
		bool mHasLoad;
	};

};
