#pragma once
#define NOMINMAX
#include <d3d11_1.h>
#include "Defines.hpp"
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
		unsigned width, height;
		TextureFormat mFormat;
	public:
		TextureResource();
		~TextureResource();
		bool Load(void * mem, size_t size);
		bool Load(std::wstring fullPath);
		void Create(unsigned width, unsigned height, TextureFormat format, char * ptr = nullptr);
		void SetTexture2D(ID3D11Texture2D * t2d);
		void SetSRV(ID3D11ShaderResourceView * srv);
		unsigned GetHeight() const
		{
			return height;
		}
		unsigned GetWidth() const
		{
			return width;
		}
		void SetWidth(unsigned w)
		{
			width = w;
		}
		void SetHeight(unsigned h)
		{
			height = h;
		}
		ID3D11Texture2D * GetTexture2D() const
		{
			return texture2d;
		}
		ID3D11ShaderResourceView * GetSRV() const
		{
			return shaderResourceView;
		}
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
		unsigned GetWidth() const;

		unsigned GetHeight() const;
		virtual void UpdateTexture() {};
		enum class ImageType
		{
			DDS,
			PNG,
			BMP
		};
		bool SaveAs(ImageType type, const std::string& path);
	private:
		
		TextureResource mTextureResource;
	};
	class DynamicTexture: public Texture
	{
	public:
		friend class Renderer;
		DynamicTexture(unsigned width, unsigned height, TextureFormat format, char * ptr);
		
		void UpdateDirtyRectangle(const Rectangle& rect);
		void ClearDirtyRectangle();
		const Rectangle& GetDirtyRectangle() const;
		virtual void UpdateTexture() override;
	private:
		Rectangle mDirtyRectangle;
		char * mPtr;
	};


};
