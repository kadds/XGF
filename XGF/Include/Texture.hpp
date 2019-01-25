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
		ID3D11ShaderResourceView * shaderResourceView;
		ID3D11Texture2D * texture2d;
		unsigned width, height;
		TextureFormat mFormat;

		TextureResource();
		~TextureResource();
		bool Load(void * mem, size_t size);
		bool Load(std::wstring fullPath);
		void Create(unsigned width, unsigned height, TextureFormat format, char * ptr = nullptr);
	};

	class Texture
	{
	public:
		friend class Renderer;
		Texture();
		virtual ~Texture();

		TextureResource& GetTextureResource();
		ID3D11ShaderResourceView* GetRawTexture() const;
		ID3D11ShaderResourceView* const * GetRawTexturePtr() const;
		ID3D11Texture2D * GetRawTexture2D() const;
		unsigned GetWidth() const;

		unsigned GetHeight() const;
		virtual void UpdateTexture() {};
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
