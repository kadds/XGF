#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
#include "Texture.hpp"

namespace XGF
{
	class GDI;
	/*
	  RenderTarget£¬äÖÈ¾Ä¿±ê
	*/
	class RenderTarget
	{
	public:
		RenderTarget();
		~RenderTarget();
		void Initialize(int textureWidth, int textureHeight, DXGI_FORMAT renderTargetFormat, int msaaQuality);
		void Initialize(ID3D11Texture2D * renderTargetTexture);
		ID3D11RenderTargetView*& GetRenderTargetView();
		ID3D11DepthStencilView*& GetDepthStencilView();
		void Shutdown();
		void Clear(const float color[]);
		void Clear(const Color & c);
		void ClearDepthStencilBuffer();
		Texture * GetTexture()
		{
			return &mRenderTargetTexture;
		}
	private:
		
		void Initialize(D3D11_TEXTURE2D_DESC & desc, ID3D11Texture2D * texture, int msaaQuality);
		ID3D11RenderTargetView* mRenderTargetView;
		ID3D11DepthStencilView * mDepthStencilView;
		ID3D11Texture2D * mDepthStencilBuffer;

		Texture mRenderTargetTexture;
		int mWidth, mHeight;
		bool mCreateTexture;
	};

};

