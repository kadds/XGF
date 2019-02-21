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
	class FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();
		void Initialize(int textureWidth, int textureHeight, DXGI_FORMAT renderTargetFormat, int msaaQuality, int targetCount, bool depthStencilViewTexture = false);
		void Initialize(ID3D11Texture2D * renderTargetTexture);
		ID3D11DepthStencilView*& GetDepthStencilView();
		void Shutdown();
		void Clear(const float color[]);
		void Clear(const Color & c);
		void ClearDepthStencilBuffer();
		void Bind();
		Texture* GetTexture(int index) const;

		Texture* GetDepthStencilTexture() const;
	private:
		void InitializeDepthView(D3D11_TEXTURE2D_DESC & desc, int msaaQuality, bool depthStencilViewTexture);

		void Initialize(D3D11_TEXTURE2D_DESC & desc, std::unique_ptr<Texture> texture, int msaaQuality);
		ID3D11DepthStencilView * mDepthStencilView;
		std::unique_ptr<Texture> mDepthStencilTexture;

		std::vector<std::pair<std::unique_ptr<Texture>, ID3D11RenderTargetView*>> mRenderTarget;
		int mWidth, mHeight;

		D3D11_VIEWPORT mViewport;
	};

};

