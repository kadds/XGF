#include "..\..\Include\RenderToTexture.hpp"
#include "..\..\Include\GDI.hpp"
#include "..\..\Include\Logger.hpp"
namespace XGF
{
	RenderToTexture::RenderToTexture() :mRenderTargetTexture(nullptr), mShaderResourceView(nullptr), mRenderTargetView(nullptr)
	{
	}


	RenderToTexture::~RenderToTexture()
	{
	}

	void RenderToTexture::Initialize(GDI * gdi, int textureWidth, int textureHeight)
	{
		mGDI = gdi;
		mHeight = textureHeight;
		mWidth = textureWidth;
		ReCreateTexture();
	}

	void RenderToTexture::Shutdown()
	{
		if (mRenderTargetTexture)
		{
			mRenderTargetTexture->Release();
			mRenderTargetTexture = nullptr;
		}
		if (mRenderTargetView)
		{
			mRenderTargetView->Release();
			mRenderTargetView = nullptr;
		}
		if (mShaderResourceView)
		{
			mShaderResourceView->Release();
			mShaderResourceView = nullptr;
		}
	}

	void RenderToTexture::SetRenderTarget()
	{
		mGDI->GetDeviceContext()->OMSetRenderTargets(1, &mRenderTargetView, mGDI->GetDepthStencilView());
	}

	void RenderToTexture::Clear(const float color[])
	{
		mGDI->GetDeviceContext()->ClearRenderTargetView(mRenderTargetView, color);

		mGDI->GetDeviceContext()->ClearDepthStencilView(mGDI->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		ID3D11ShaderResourceView *const pSRV[1] = { nullptr };
		mGDI->GetDeviceContext()->PSSetShaderResources(0, 1, pSRV);
	}
	void RenderToTexture::Clear(const Color & c)
	{
		float color[4];
		color[0] = c.x;
		color[1] = c.y;
		color[2] = c.z;
		color[3] = c.w;
		Clear(color);
	}
	void RenderToTexture::SetDefaultRenderTarget()
	{
		mGDI->SetRenderTargetView();
	}
	ID3D11ShaderResourceView* RenderToTexture::GetShaderResourceView()
	{
		return mShaderResourceView;
	}
	void RenderToTexture::ReCreateTexture()
	{
		if (mRenderTargetTexture)
			mRenderTargetTexture->Release();
		if (mRenderTargetView)
			mRenderTargetView->Release();
		if (mShaderResourceView)
			mShaderResourceView->Release();

		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = mWidth;
		textureDesc.Height = mHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		auto q = mGDI->Query4xMsaaQuality();
		auto enable = mGDI->IsEnable4xMsaa();
		if(enable && q > 0)
		{
			textureDesc.SampleDesc.Count = 4;
			textureDesc.SampleDesc.Quality = q - 1;
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
		XGF_Error_Check(Render, mGDI->GetDevice()->CreateTexture2D(&textureDesc, NULL, &mRenderTargetTexture), "CT2D Failed");
		renderTargetViewDesc.Format = textureDesc.Format;
		if(enable && q > 0)
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		else
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		renderTargetViewDesc.Texture2D.MipSlice = 0;

		XGF_Error_Check(Render, mGDI->GetDevice()->CreateRenderTargetView(mRenderTargetTexture, &renderTargetViewDesc, &mRenderTargetView), "CreateRT Failed");

		shaderResourceViewDesc.Format = textureDesc.Format;
		if(enable && q > 0)
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		else
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		XGF_Error_Check(Render, mGDI->GetDevice()->CreateShaderResourceView(mRenderTargetTexture, &shaderResourceViewDesc, &mShaderResourceView), "CreateSRV Failed");

	}
};
