#include "..\..\Include\FrameBuffer.hpp"
#include "..\..\Include\GDI.hpp"
#include "..\..\Include\Logger.hpp"
#include "../../Include/Context.hpp"

namespace XGF
{
	FrameBuffer::FrameBuffer() : mDepthStencilTexture(nullptr), mDepthStencilView(nullptr)
	{
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}


	FrameBuffer::~FrameBuffer()
	{
	}
	void FrameBuffer::Initialize(int textureWidth, int textureHeight, DXGI_FORMAT format, DepthStencilFormat depthStencilFormat, int msaaQuality, int targetCount, bool depthStencilViewTexture)
	{
		mHeight = textureHeight;
		mWidth = textureWidth;
		mViewport.Height = static_cast<float>(mHeight);
		mViewport.Width = static_cast<float>(mWidth);
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		
		D3D11_TEXTURE2D_DESC textureDesc;
		memset(&textureDesc, 0, sizeof(textureDesc));
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
		int i = 0;
		for (; i < targetCount; i++)
		{
			ID3D11Texture2D * texture;

			XGF_Error_Check(Render, gdi.GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture), "Create Texture2D Failed");
			PutDebugString(texture);
			auto renderTarget = std::make_unique<Texture>();
			renderTarget->GetTextureResource().SetTexture2D(texture);
			renderTarget->GetTextureResource().SetWidth(textureWidth);
			renderTarget->GetTextureResource().SetHeight(textureHeight);
			Initialize(textureDesc, std::move(renderTarget), msaaQuality);
		}
		InitializeDepthView(textureDesc, depthStencilFormat, textureDesc.SampleDesc.Quality, depthStencilViewTexture);

	}

	void FrameBuffer::Initialize(ID3D11Texture2D* renderTargetTexture, DepthStencilFormat depthStencilFormat)
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		renderTargetTexture->GetDesc(&textureDesc);
		mHeight = textureDesc.Height;
		mWidth = textureDesc.Width;
		mViewport.Height = static_cast<float>(textureDesc.Height);
		mViewport.Width = static_cast<float>(textureDesc.Width);
		renderTargetTexture->AddRef();
		auto renderTarget = std::make_unique<Texture>();
		renderTarget->GetTextureResource().SetTexture2D(renderTargetTexture);
		renderTarget->GetTextureResource().SetWidth(mWidth);
		renderTarget->GetTextureResource().SetHeight(mHeight);
		Initialize(textureDesc, std::move(renderTarget), textureDesc.SampleDesc.Quality);
		InitializeDepthView(textureDesc, depthStencilFormat, textureDesc.SampleDesc.Quality, false);
	}

	ID3D11DepthStencilView*& FrameBuffer::GetDepthStencilView()
	{
		return mDepthStencilView;
	}


	void FrameBuffer::Initialize(D3D11_TEXTURE2D_DESC& desc, std::unique_ptr<Texture> texture, int msaaQuality)
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
		int i = 0;
		ID3D11RenderTargetView * renderTargetView;
		XGF_Error_Check(Render, gdi.GetDevice()->CreateRenderTargetView(texture->GetRawTexture2D(), &renderTargetViewDesc, &renderTargetView), "Create Rendertarget Failed");
		PutDebugString(renderTargetView);
		
		if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			shaderResourceViewDesc.Format = desc.Format;
			if (msaaQuality > 0)
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			else
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			ID3D11ShaderResourceView * srv = 0;
			XGF_Error_Check(Render, gdi.GetDevice()->CreateShaderResourceView(texture->GetRawTexture2D(), &shaderResourceViewDesc, &srv), "Create SRV Failed");
			PutDebugString(srv);
			texture->GetTextureResource().SetSRV(srv);
		}
		mRenderTarget.push_back(std::make_pair(std::move(texture), renderTargetView));
	}

	void FrameBuffer::Shutdown()
	{
		for (auto & var : mRenderTarget)
		{
			var.second->Release();
		}
		if (mDepthStencilView)
		{
			mDepthStencilView->Release();
			mDepthStencilView = nullptr;
		}
		mDepthStencilTexture.reset();
		mRenderTarget.clear();
	}

	void FrameBuffer::Clear(const float color[])
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		for (auto & renderTarget : mRenderTarget)
		{
			gdi.GetDeviceContext()->ClearRenderTargetView(renderTarget.second, color);
		}
	}
	void FrameBuffer::Clear(const Color & c)
	{
		Clear(&c.x);
	}

	void FrameBuffer::ClearDepthStencilBuffer()
	{
		Context::Current().QueryGraphicsDeviceInterface().GetDeviceContext()->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void FrameBuffer::Bind()
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();
		ID3D11RenderTargetView * targets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
		auto target2 = targets;
		for (auto & renderTargetTexture : mRenderTarget)
		{
			*target2++ = renderTargetTexture.second;
		}
		gdi.GetDeviceContext()->OMSetRenderTargets(mRenderTarget.size(), targets, mDepthStencilView);
		
		gdi.GetDeviceContext()->RSSetViewports(1, &mViewport);
	}

	Texture* FrameBuffer::GetTexture(int index) const
	{
		return mRenderTarget[index].first.get();
	}

	Texture* FrameBuffer::GetDepthStencilTexture() const
	{
		return mDepthStencilTexture.get();
	}

	void FrameBuffer::InitializeDepthView(D3D11_TEXTURE2D_DESC& desc, DepthStencilFormat depthFormat, int msaaQuality, bool depthStencilViewTexture)
	{
		auto& gdi = Context::Current().QueryGraphicsDeviceInterface();
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

		memset(&depthBufferDesc, 0, sizeof(depthBufferDesc));

		depthBufferDesc.Width = desc.Width;
		depthBufferDesc.Height = desc.Height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		switch (depthFormat) { 
			case DepthStencilFormat::Depth16Float: depthBufferDesc.Format = DXGI_FORMAT_D16_UNORM; break;
			case DepthStencilFormat::Depth32Float: depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT; break;
			case DepthStencilFormat::Depth24FloatStencil8Uint: depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; break;
		default: ;
		}
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
		if (depthStencilViewTexture)
		{
			switch (depthFormat) {
			case DepthStencilFormat::Depth16Float: depthBufferDesc.Format = DXGI_FORMAT_R16_TYPELESS; break;
			case DepthStencilFormat::Depth32Float: depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS; break;
			case DepthStencilFormat::Depth24FloatStencil8Uint: depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; break;
			default:;
			}
			depthBufferDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		ID3D11Texture2D * tex2d;
		XGF_Error_Check(Framework, gdi.GetDevice()->CreateTexture2D(&depthBufferDesc, NULL, &tex2d), "Create DepthStencilBuffer Error");
		PutDebugString(tex2d);
		
		mDepthStencilTexture = std::make_unique<Texture>();
		mDepthStencilTexture->GetTextureResource().SetTexture2D(tex2d);
		if (depthStencilViewTexture)
		{
			
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			memset(&shaderResourceViewDesc, 0, sizeof(shaderResourceViewDesc));
			switch (depthFormat) {
			case DepthStencilFormat::Depth16Float: shaderResourceViewDesc.Format = DXGI_FORMAT_R16_UNORM; break;
			case DepthStencilFormat::Depth32Float:shaderResourceViewDesc.Format = DXGI_FORMAT_R32_UINT; break;
			case DepthStencilFormat::Depth24FloatStencil8Uint: shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
			default:;
			}
			if (msaaQuality > 0)
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			else
				shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			ID3D11ShaderResourceView * srv = 0;
			XGF_Error_Check(Render, gdi.GetDevice()->CreateShaderResourceView(tex2d, &shaderResourceViewDesc, &srv), "Create SRV Failed");
			PutDebugString(srv);
			mDepthStencilTexture->GetTextureResource().SetSRV(srv);
		}

		memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));
		switch (depthFormat) {
		case DepthStencilFormat::Depth16Float: depthStencilViewDesc.Format = DXGI_FORMAT_D16_UNORM; break;
		case DepthStencilFormat::Depth32Float:depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT; break;
		case DepthStencilFormat::Depth24FloatStencil8Uint: depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; break;
		default:;
		}
		if (msaaQuality > 0)
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		else
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		depthStencilViewDesc.Texture2D.MipSlice = 0;
		XGF_Error_Check(Framework, gdi.GetDevice()->CreateDepthStencilView(tex2d,
			&depthStencilViewDesc, &mDepthStencilView), "CreateDepthStencilView Error");

		PutDebugString(mDepthStencilView);
	}
};
