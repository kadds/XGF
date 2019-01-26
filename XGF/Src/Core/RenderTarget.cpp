#include "..\..\Include\RenderTarget.hpp"
#include "..\..\Include\GDI.hpp"
#include "..\..\Include\Logger.hpp"
#include "../../Include/Context.hpp"

namespace XGF
{
	RenderTarget::RenderTarget() : mRenderTargetView(nullptr)
	{
	}


	RenderTarget::~RenderTarget()
	{
	}
	void RenderTarget::Initialize(int textureWidth, int textureHeight, DXGI_FORMAT format, int msaaQuality)
	{
		mHeight = textureHeight;
		mWidth = textureWidth;

		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = mWidth;
		textureDesc.Height = mHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;
		if (msaaQuality > 0)
		{
			textureDesc.SampleDesc.Count = 4;
			textureDesc.SampleDesc.Quality = msaaQuality - 1;
		}
		else
		{
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
		}
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		ID3D11Texture2D * texture;
		XGF_Error_Check(Render, gdi.GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture), "CT2D Failed");
		PutDebugString(texture);
		Initialize(textureDesc, texture,  msaaQuality);

		mCreateTexture = true;
	}

	void RenderTarget::Initialize(ID3D11Texture2D* renderTargetTexture)
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		renderTargetTexture->GetDesc(&textureDesc);
		mHeight = textureDesc.Height;
		mWidth = textureDesc.Width;
		Initialize(textureDesc, renderTargetTexture, textureDesc.SampleDesc.Quality);

		mCreateTexture = false;
	}

	ID3D11RenderTargetView*& RenderTarget::GetRenderTargetView()
	{
		return mRenderTargetView;
	}

	ID3D11DepthStencilView*& RenderTarget::GetDepthStencilView()
	{
		return mDepthStencilView;
	}


	void RenderTarget::Initialize(D3D11_TEXTURE2D_DESC& desc, ID3D11Texture2D * renderTargetTexture, int msaaQuality)
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		renderTargetViewDesc.Format = desc.Format;
		if (msaaQuality > 0)
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		else
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		renderTargetViewDesc.Texture2D.MipSlice = 0;
		XGF_Error_Check(Render, gdi.GetDevice()->CreateRenderTargetView(renderTargetTexture, &renderTargetViewDesc, &mRenderTargetView), "CreateRT Failed");
		PutDebugString(renderTargetTexture);
		mRenderTargetTexture.GetTextureResource().texture2d = renderTargetTexture;
		mRenderTargetTexture.GetTextureResource().width = desc.Width;
		mRenderTargetTexture.GetTextureResource().height = desc.Height;
		if(desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			shaderResourceViewDesc.Format = desc.Format;
			if (msaaQuality > 0)
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			else
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			ID3D11ShaderResourceView * srv = 0;
			XGF_Error_Check(Render, gdi.GetDevice()->CreateShaderResourceView(renderTargetTexture, &shaderResourceViewDesc, &srv), "CreateSRV Failed");
			PutDebugString(srv);
			mRenderTargetTexture.GetTextureResource().shaderResourceView = srv;
		}

		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		depthBufferDesc.Width = desc.Width;
		depthBufferDesc.Height = desc.Height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (msaaQuality > 0)
		{
			depthBufferDesc.SampleDesc.Count = 4;
			depthBufferDesc.SampleDesc.Quality = msaaQuality - 1;
		}
		else
		{
			depthBufferDesc.SampleDesc.Count = 1;
			depthBufferDesc.SampleDesc.Quality = 0;
		}
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		XGF_Error_Check(Framework, gdi.GetDevice()->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer), "CreatemDepthStencilBuffer Error");
		PutDebugString(mDepthStencilBuffer);

		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = depthBufferDesc.Format;
		if (msaaQuality > 0)
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		else
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		depthStencilViewDesc.Texture2D.MipSlice = 0;
		XGF_Error_Check(Framework, gdi.GetDevice()->CreateDepthStencilView(mDepthStencilBuffer,
			&depthStencilViewDesc, &mDepthStencilView), "CreateDepthStencilView Error");

		PutDebugString(mDepthStencilView);

	}

	void RenderTarget::Shutdown()
	{
		if (mRenderTargetTexture.GetTextureResource().texture2d)
		{
			if(mCreateTexture)
				mRenderTargetTexture.GetTextureResource().texture2d->Release();
			mRenderTargetTexture.GetTextureResource().texture2d = nullptr;
		}
		if (mRenderTargetView)
		{
			mRenderTargetView->Release();
			mRenderTargetView = nullptr;
		}
		if (mRenderTargetTexture.GetTextureResource().shaderResourceView)
		{
			mRenderTargetTexture.GetTextureResource().shaderResourceView->Release();
			mRenderTargetTexture.GetTextureResource().shaderResourceView = nullptr;
		}

		if (mDepthStencilView)
		{
			mDepthStencilView->Release();
			mDepthStencilView = nullptr;
		}

		if (mDepthStencilBuffer)
		{
			mDepthStencilBuffer->Release();
			mDepthStencilBuffer = nullptr;
		}
	}

	void RenderTarget::Clear(const float color[])
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		gdi.GetDeviceContext()->ClearRenderTargetView(mRenderTargetView, color);
		ClearDepthStencilBuffer();
	}
	void RenderTarget::Clear(const Color & c)
	{
		float color[4];
		color[0] = c.x;
		color[1] = c.y;
		color[2] = c.z;
		color[3] = c.w;
		Clear(color);
	}

	void RenderTarget::ClearDepthStencilBuffer()
	{
		Context::Current().QueryGraphicsDeviceInterface().GetDeviceContext()->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	}
};
