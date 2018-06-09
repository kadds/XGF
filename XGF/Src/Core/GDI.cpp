#include "../../Include/GDI.hpp"
#include "../../Include/Log.hpp"
#include <DirectXMath.h>
namespace XGF
{
	void GDI::Create()
	{
		mDisplayMode = DisplayMode::Windowed;
		mDisplayFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		XGF_Error_Check(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**) (&mFactory1)), "CreateDXGIFactory Failed!");
		XGF_Info_Check(mFactory1->QueryInterface(__uuidof(IDXGIFactory2), (void**) (&mFactory2)), "Find DXGIFactory2 Failed! Use DXGIFactory1 Now!");

		PutDebugString(mFactory1);
		if (mFactory2)
		{
			PutDebugString(mFactory2);
		}
		//枚举系统中的适配器
		UINT adapter_no = 0;
		IDXGIAdapter1* dxgi_adapter = nullptr;
		DXGI_ADAPTER_DESC1 dc;
		int i = 0;
		int j = 0;
		while (mFactory1->EnumAdapters1(i, &dxgi_adapter) != DXGI_ERROR_NOT_FOUND)
		{
			if (dxgi_adapter != nullptr)
			{
				dxgi_adapter->GetDesc1(&dc);
				XGF_ReportDebug_Ex(L"Adapter List", "Adpapter:", i, ",DeviceID:", dc.DeviceId, ",DedicatedSystemMemory:", (dc.DedicatedSystemMemory >> 20), "MB,DedicatedVideoMemory", (dc.DedicatedVideoMemory >> 20)
					, "MB,SharedSystemMemory:", dc.SharedSystemMemory >> 20, "MB,AdapterLuid:", dc.AdapterLuid.LowPart, ",Revision:", dc.Revision, ",VendorId:", dc.VendorId, ",SubSysId:", dc.SubSysId, ",Description:", ((wchar_t *) dc.Description));

				mAdapters.push_back(dxgi_adapter);

				IDXGIOutput * output;
				DXGI_OUTPUT_DESC desc;
				j = 0;
				while (dxgi_adapter->EnumOutputs(j++, &output) != DXGI_ERROR_NOT_FOUND)
				{
					output->GetDesc(&desc);
					XGF_ReportDebug_Ex("OutPut:", j, ",name:", desc.DeviceName);
					mOutputs.push_back(output);
					SaveDisplayMode(j, output);
				}
			}
			++i;
		}
		if (mAdapters.empty())
		{
			XGF_ReportError0("None Adapters");
		}
		XGF_Warn_Check(mFactory1->MakeWindowAssociation(mTopHwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES), "MakeWindowAssociation Error");
		int flag = 0;
#ifdef _DEBUG
		flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		// view https://msdn.microsoft.com/zh-cn/library/windows/apps/ff476082.aspx#code-snippet-2
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0, };
		D3D_FEATURE_LEVEL   curLevel;
		D3D_FEATURE_LEVEL featureLevels111[] = {
			D3D_FEATURE_LEVEL_11_1
		};
		if (FAILED(D3D11CreateDevice(mAdapters[0], D3D_DRIVER_TYPE_UNKNOWN, 0, flag, featureLevels111, 1, D3D11_SDK_VERSION, &mD3dDevice, &curLevel, &mDeviceContext)))
		{
			XGF_Error_Check(D3D11CreateDevice(mAdapters[0], D3D_DRIVER_TYPE_UNKNOWN, 0, flag, featureLevels, sizeof(featureLevels) / sizeof(featureLevels[0]), D3D11_SDK_VERSION, &mD3dDevice, &curLevel, &mDeviceContext)
				, "CreateDevice Error");
		}
		else
		{
			XGF_ReportDebug0("We can use d3d11.1");
		}
		UINT  m4xMsaaQuality = 1;
		XGF_Warn_Check(mD3dDevice->CheckMultisampleQualityLevels(
			mDisplayFormat, 4, &m4xMsaaQuality), "CheckMultisampleQualityLevels Warn");
		
#ifdef _DEBUG
		char const *levelstr[] = {"11","10.1","10" };
		for (size_t i = 0; i < sizeof(featureLevels) / sizeof(featureLevels[0]); i++)
		{
			if (curLevel == featureLevels[i])
			{
				XGF_ReportDebug_Ex("D3D Level:", levelstr[i]);
				break;
			}
		}
#endif
		mFeatureLevel = curLevel;
		XGF_ReportDebug_Ex("4xMsaaQuality:", m4xMsaaQuality);
		PutDebugString(mDeviceContext);
		PutDebugString(mD3dDevice);
		CreateSwapChain();
		
		
		memset(mSamplerState, 0, sizeof(mSamplerState));
		memset(mBlendState, 0, sizeof(mBlendState));
		memset(mDepthStencilState, 0, sizeof(mDepthStencilState));
		memset(mRasterizerState, 0, sizeof(mRasterizerState));
		
		SizeChanged(mWidth, mHeight);
	}
	
	void GDI::SaveDisplayMode(int c, IDXGIOutput * pDXGIOutput)
	{
		UINT  num = 0;
		pDXGIOutput->GetDisplayModeList(mDisplayFormat, 0, &num, 0);
		DXGI_MODE_DESC * pdsk = new DXGI_MODE_DESC[num];
		mScreenMode.push_back(std::make_pair(num, pdsk));
		pDXGIOutput->GetDisplayModeList(mDisplayFormat, 0, &num, pdsk);

		XGF_ReportDebug_Ex("DisplayMode has ", num);
	}
	void GDI::Destroy()
	{
		mSwapChain->SetFullscreenState(false, nullptr);
		
		for (int i = 0; i < (int)BlendState::InvalidValue; i++)
		{
			if(mBlendState[i] != nullptr)
				mBlendState[i]->Release();
		}
		for (int i = 0; i < (int)SamplerState::InvalidValue; i++)
		{
			if (mSamplerState[i] != nullptr)
				mSamplerState[i]->Release();
		}
		for (int i = 0; i < (int)DepthStencilState::InvalidValue; i++)
		{
			if (mDepthStencilState[i] != nullptr)
				mDepthStencilState[i]->Release();
		}

		for (int i = 0; i < (int)RasterizerState::InvalidValue; i++)
		{
			if (mRasterizerState[i] != nullptr)
				mRasterizerState[i]->Release();
		}

		if (mDepthStencilView)
			mDepthStencilView->Release();
		mDepthStencilView = nullptr;
		if (mDepthStencilBuffer)
			mDepthStencilBuffer->Release();
		mDepthStencilBuffer = nullptr;
		if (mRenderTargetView)
			mRenderTargetView->Release();
		mRenderTargetView = nullptr;
		
		for each (auto var in mAdapters)
		{
			var->Release();
		}
		for each (auto var in mOutputs)
		{
			var->Release();
		}
		for each (auto var in mScreenMode)
		{
			delete[] var.second;
		}

		if (mSwapChain1)
			mSwapChain1->Release();
		mSwapChain1 = nullptr;
		if (mSwapChain)
			mSwapChain->Release();
		mSwapChain = nullptr;
		if (mDeviceContext)
			mDeviceContext->Release();
		mDeviceContext = nullptr;
		if (mFactory1)
			mFactory1->Release();
		if (mFactory2)
			mFactory2->Release();
#ifdef _DEBUG
		QueryDebugInterface();
#endif
		if (mD3dDevice)
			mD3dDevice->Release();
		mD3dDevice = nullptr;

	}

	void GDI::Clear(const float color[])
	{
		mDeviceContext->ClearRenderTargetView(mRenderTargetView, color);
		//清除深度缓冲.  
		mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//把ShaderView置空
		//TODO:Bug:1000
		ID3D11ShaderResourceView *const pSRV[1] = { nullptr };

		mDeviceContext->PSSetShaderResources(0, 1, pSRV);
	}
	void GDI::Clear(const SM::Color & c)
	{
		if (!mRTTs.empty())
		{
			mRTTs.top()->Clear(c);
			return;
		}
		float color[4];
		color[0] = c.x;
		color[1] = c.y;
		color[2] = c.z;
		color[3] = c.w;
		Clear(color);
	}
	void GDI::ClearDepthStencilBuffer()
	{
		mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	}

	void GDI::Present(bool isVsync)
	{
		if (!mIsStandby)
		{
			
			HRESULT  hr;
			if (mSwapChain1) {
				DXGI_PRESENT_PARAMETERS dpp;
				dpp.DirtyRectsCount = 0;
				dpp.pScrollOffset = 0;
				dpp.pScrollRect = nullptr;
				hr = mSwapChain1->Present1(isVsync, 0, &dpp);
			}
			else {
				hr = mSwapChain->Present(isVsync, 0);
			}
			if(hr == DXGI_STATUS_OCCLUDED)
			{
				mIsStandby = true;
				XGF_ReportDebug("ERROR IN Present! ", "DXGI_STATUS_OCCLUDED");
				ShowWindow(mHwnd, SW_MINIMIZE);
				CheckFullScreenForce(false);
			}
		}
		else
		{
			HRESULT  hr;
			hr = mSwapChain->Present(isVsync, DXGI_PRESENT_TEST);
			if (hr == S_OK)
			{
				mIsStandby = false;
			}
		}

	}

	void GDI::Initialize(HINSTANCE instance, HWND WndHwnd, HWND TopHwnd, UINT ClientWidth, UINT ClientHeight)
	{
		mTopHwnd = TopHwnd;
		mInstance = instance;
		mHwnd = WndHwnd;
		mWidth = ClientWidth;
		mHeight = ClientHeight;
	}

	void GDI::SizeChanged(UINT ClientWidth, UINT ClientHeight)
	{
		if (!mSwapChain1 && !mSwapChain)
			return;
		XGF_ReportDebug_Ex("SIZE Changed:", ClientWidth, ",", ClientHeight);
		if (ClientWidth < 1)
			ClientWidth = 1;
		if (ClientHeight < 1)
			ClientHeight = 1;

		if (mRenderTargetView)
		{
			mRenderTargetView->Release();
			mRenderTargetView = 0;
		}
		if (mDepthStencilView) {
			mDepthStencilView->Release();
			mDepthStencilView = 0;
		}

		if (mDepthStencilBuffer)
		{
			mDepthStencilBuffer->Release();
			mDepthStencilBuffer = 0;
		}
		XGF_Error_Check(mSwapChain->ResizeBuffers(1, ClientWidth, ClientHeight, mDisplayFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH), "ResizeBuffers Error");
		PutDebugString(mSwapChain);
		ID3D11Texture2D* backBufferPtr;
		XGF_Error_Check(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr), "GetSwapChainBuffer is Error");

		XGF_Error_Check(mD3dDevice->CreateRenderTargetView(backBufferPtr, NULL, &mRenderTargetView), "CreateRenderTargetView is Error");

		backBufferPtr->Release();
		backBufferPtr = 0;
		PutDebugString(mRenderTargetView);

		D3D11_TEXTURE2D_DESC depthBufferDesc;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		depthBufferDesc.Width = ClientWidth;
		depthBufferDesc.Height = ClientHeight;
		depthBufferDesc.MipLevels = 1;  
		depthBufferDesc.ArraySize = 1;  
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;
		XGF_Error_Check(mD3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer), "CreatemDepthStencilBuffer Error");
		PutDebugString(mDepthStencilBuffer);
		// 初始化深度模版视图.
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		XGF_Error_Check(mD3dDevice->CreateDepthStencilView(mDepthStencilBuffer,
			&depthStencilViewDesc, &mDepthStencilView), "CreateDepthStencilView Error");

		PutDebugString(mDepthStencilView);
		mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

		D3D11_VIEWPORT vp;
		vp.MinDepth = 0.f;
		vp.MaxDepth = 1.f;
		vp.TopLeftX = 0.f;
		vp.TopLeftY = 0.f;
		vp.Height = static_cast<FLOAT>(ClientHeight);
		vp.Width = static_cast<FLOAT>(ClientWidth);
		mDeviceContext->RSSetViewports(1, &vp);

		mWidth = ClientWidth;
		mHeight = ClientHeight;
	}

	void GDI::ResizeTarget(UINT x, UINT y)
	{
		BOOL bl;
		mSwapChain->GetFullscreenState(&bl, NULL);
		DXGI_MODE_DESC mode;
		mode.Width = x;
		mode.Height = y;
		mode.Format = mDisplayFormat;
		mode.RefreshRate.Numerator = 0;
		mSwapChain->ResizeTarget(&mode);
	}

	void GDI::SetFullScreen(bool isFullscreen, int pos)
	{
		BOOL bl;
		mSwapChain->GetFullscreenState(&bl, NULL);
		if (isFullscreen && bl == FALSE)
		{
			GetClientRect(mHwnd, &mLastWinRc);
			DXGI_MODE_DESC dxm = mScreenMode.at(0).second[pos];
			dxm.Format = DXGI_FORMAT_UNKNOWN;
			XGF_ReportInfo0("full screen");
			XGF_Warn_Check(mSwapChain->SetFullscreenState(isFullscreen, 0), "Can't switch fullscreenState");
			XGF_Warn_Check(mSwapChain->ResizeTarget(&dxm), "ResizeTarget Error");
		}
		else if (isFullscreen && bl == TRUE)
		{
			return;
		}
		else if (!isFullscreen && bl == TRUE)
		{
			mSwapChain->SetFullscreenState(isFullscreen, 0);
			DXGI_MODE_DESC dxm = mScreenMode.at(0).second[0];
			dxm.Width = mLastWinRc.right;
			dxm.Height = mLastWinRc.bottom;
			dxm.Format = mDisplayFormat;
			dxm.RefreshRate.Numerator = 0;
			mSwapChain->ResizeTarget(&dxm);
		}
	}
#ifdef _DEBUG
	void GDI::QueryDebugInterface()
	{
		ID3D11Debug *d3dDebug;
		mD3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
		d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
#endif

	HWND GDI::GetHwnd()
	{
		return mHwnd;
	}

	HWND GDI::GetTopHwnd()
	{
		return mTopHwnd;
	}

	HINSTANCE GDI::GetInstance()
	{
		return mInstance;
	}

	void GDI::SetRenderTargetView()
	{
		mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	}

	bool GDI::SetDisplayMode(DisplayMode dm, int left, int top, int cx, int cy, bool move, bool isClientSize)
	{
		mLastMode = mDisplayMode;
		mDisplayMode = dm;
		if (dm == DisplayMode::Windowed)
		{
			SetFullScreen(false, 0);
			UINT newtype = ::GetWindowLong(mHwnd, GWL_STYLE);
			if (!(newtype & WS_CAPTION))
			{
				newtype |= (WS_CAPTION | WS_SYSMENU | WS_SIZEBOX);
				::SetWindowLong(mHwnd, GWL_STYLE, newtype);
			}

			RECT rect = { 0,0,cx,cy };
			if (!isClientSize)
			{
				AdjustWindowRect(&rect, newtype, FALSE);
			}
			SetWindowPos(mHwnd, NULL, left, top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | (!move ? SWP_NOMOVE | SWP_NOZORDER : SWP_NOZORDER));
			ResizeTarget(rect.right - rect.left, rect.bottom - rect.top);
			return true;
		}
		else if (dm == DisplayMode::Borderless)
		{
			SetFullScreen(false, 0);
			UINT newtype = ::GetWindowLong(mHwnd, GWL_STYLE);
			if (newtype & WS_CAPTION)
			{
				newtype &= ~WS_CAPTION & ~WS_SYSMENU & ~WS_SIZEBOX;
				::SetWindowLong(mHwnd, GWL_STYLE, newtype);
			}
			RECT rect = { 0,0,cx,cy };
			if (!isClientSize)
			{
				AdjustWindowRect(&rect, newtype, FALSE);
			}
			SetWindowPos(mHwnd, 0, left, top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | (!move ? SWP_NOMOVE | SWP_NOZORDER : SWP_NOZORDER));
			ResizeTarget(rect.right - rect.left, rect.bottom - rect.top);
			return true;
		}
		else
		{
			if (mScreenMode.empty()) return false;
			auto var = mScreenMode[0];
			for (int i = 0; i < var.first; i++)
			{
				DXGI_MODE_DESC * k = &var.second[i];
				if (k->Height == cy && k->Width == cx
					&& k->Format == mDisplayFormat && k->ScanlineOrdering == DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE && k->Scaling == 0
					&& k->RefreshRate.Numerator / k->RefreshRate.Denominator > 40)
				{
					mFullScreenSize.cx = cx;
					mFullScreenSize.cy = cy;
					SetFullScreen(true, i);
					return true;
				}
			}
			return false;
		}
		return true;
	}


	void GDI::CheckFullScreenForce(bool isforce)
	{
		BOOL bl;

		mSwapChain->GetFullscreenState(&bl, NULL);
		if (isforce)
		{
			if (bl == FALSE && mDisplayMode == DisplayMode::FullScreen)
			{
				SetDisplayMode(DisplayMode::FullScreen, 0, 0, mFullScreenSize.cx, mFullScreenSize.cy, true);
			}
		}
		else
		{
			if (mDisplayMode == DisplayMode::FullScreen)
			{
				SetDisplayMode(mLastMode, mLastWinRc.left, mLastWinRc.top, mLastWinRc.right, mLastWinRc.bottom, true, true);
				mDisplayMode = DisplayMode::FullScreen;
			}
		}
	}

	ID3D11BlendState * GDI::GetRawBlendState(BlendState bs)
	{
		if (mBlendState[(int)bs] == nullptr)
		{
			if (bs != BlendState::InvalidValue)
			{
				//创建Blend
				D3D11_BLEND_DESC blendDesc;
				ZeroMemory(&blendDesc, sizeof(blendDesc));
				blendDesc.RenderTarget[0].BlendEnable = TRUE;
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
				blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
				blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
				blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
				blendDesc.AlphaToCoverageEnable = FALSE;
				blendDesc.IndependentBlendEnable = FALSE;
				if (bs == BlendState::NoneBlend)
				{
					blendDesc.RenderTarget[0].BlendEnable = FALSE;
					mD3dDevice->CreateBlendState(&blendDesc, &mBlendState[(int)bs]);
					PutDebugString(mBlendState[(int)bs]);
					return mBlendState[(int)bs];
				}
				if (bs == BlendState::AddZeroOneAdd)
				{
					blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
					mD3dDevice->CreateBlendState(&blendDesc, &mBlendState[(int)bs]);
					PutDebugString(mBlendState[(int)bs]);
					return mBlendState[(int)bs];
				}
				if (bs == BlendState::AddOneOneAdd)
				{
					mD3dDevice->CreateBlendState(&blendDesc, &mBlendState[(int)bs]);
					PutDebugString(mBlendState[(int)bs]);
					return mBlendState[(int)bs];
				}
				XGF_ReportWarn0("BlendState is incorrect");
			}
			else
				XGF_ReportWarn0("BlendState is a invalid value!");
			
		}
		
		return mBlendState[(int)bs];
		
		
	}

	ID3D11SamplerState * GDI::GetRawSamplerState(SamplerState ss)
	{
		if (mSamplerState[(int)ss] == nullptr)
		{
			if (ss != SamplerState::InvalidValue)
			{
				D3D11_SAMPLER_DESC samplerDesc;
				samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				samplerDesc.MipLODBias = 0.0f;
				samplerDesc.MaxAnisotropy = 1;
				samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				samplerDesc.BorderColor[0] = 0;
				samplerDesc.BorderColor[1] = 0;
				samplerDesc.BorderColor[2] = 0;
				samplerDesc.BorderColor[3] = 0;
				samplerDesc.MinLOD = 0;
				samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

				if (ss == SamplerState::LineWrap)
				{
					mD3dDevice->CreateSamplerState(&samplerDesc, &mSamplerState[(int)ss]);
					PutDebugString(mSamplerState[(int)ss]);
					return mSamplerState[(int)ss];
				}
				if (ss == SamplerState::LineClamp)
				{
					samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
					samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
					samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
					mD3dDevice->CreateSamplerState(&samplerDesc, &mSamplerState[(int)ss]);
					PutDebugString(mSamplerState[(int)ss]);
					return mSamplerState[(int)ss];
				}
				if (ss == SamplerState::LineMirror)
				{
					samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
					samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
					samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
					mD3dDevice->CreateSamplerState(&samplerDesc, &mSamplerState[(int)ss]);
					PutDebugString(mSamplerState[(int)ss]);
					return mSamplerState[(int)ss];
				}
				XGF_ReportWarn0("SamplerState is incorrect");
				return mSamplerState[0];
			}
			else
				XGF_ReportWarn0("SamplerState is a invalid value!");
		}
		return mSamplerState[(int)ss];
		
	}

	ID3D11DepthStencilState * GDI::GetRawDepthStencilState(DepthStencilState ds)
	{
		if (mDepthStencilState[(int)ds] == nullptr )
		{
			if (ds != DepthStencilState::InvalidValue)
			{
				D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
				ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
				depthStencilDesc.DepthEnable = true;
				depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//D3D11_DEPTH_WRITE_MASK_ZERO禁止写深度缓冲 
				depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
				depthStencilDesc.StencilEnable = true;
				depthStencilDesc.StencilReadMask = 0xFF;
				depthStencilDesc.StencilWriteMask = 0xFF;
				// 对于front face 像素使用的模版操作操作. 
				depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
				depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				// 对于back face像素使用的模版操作模式. 
				depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
				depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
				depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				if (ds == DepthStencilState::DepthEnable)
				{
					XGF_Error_Check(mD3dDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState[(int)ds]), "CreateDepthStencilState Error");
					PutDebugString(mDepthStencilState[(int)ds]);
					return mDepthStencilState[(int)ds];
				}
					
				// 设置深度模版状态. 
				if (ds == DepthStencilState::DepthDisable)
				{
					depthStencilDesc.DepthEnable = false;
					XGF_Error_Check(mD3dDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState[(int)ds]), "CreateDepthStencilState Error");
					PutDebugString(mDepthStencilState[(int)ds]);
					return mDepthStencilState[(int)ds];
				}
				if (ds == DepthStencilState::DepthEnableWithLessEqual)
				{
					depthStencilDesc.DepthEnable = true;
					depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
					XGF_Error_Check(mD3dDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState[(int)ds]), "CreateDepthStencilState Error");
					PutDebugString(mDepthStencilState[(int)ds]);
					return mDepthStencilState[(int)ds];
				}
				XGF_ReportWarn0("DepthStencilState is incorrect");
				return mDepthStencilState[0];
			}
			else XGF_ReportWarn0("DepthStencilState is a invalid value!");

		}
		return mDepthStencilState[(int) ds];
	}

	ID3D11RasterizerState * GDI::GetRasterizerState(RasterizerState rs)
	{
		if (mRasterizerState[(int) rs] == nullptr)
		{
			if (rs != RasterizerState::InvalidValue)
			{
				D3D11_RASTERIZER_DESC rasterDesc;

				rasterDesc.AntialiasedLineEnable = false;
				rasterDesc.CullMode = D3D11_CULL_BACK;
				rasterDesc.DepthBias = 0;
				rasterDesc.DepthBiasClamp = 0.0f;
				rasterDesc.DepthClipEnable = true;
				rasterDesc.FillMode = D3D11_FILL_SOLID;
				rasterDesc.FrontCounterClockwise = false;
				rasterDesc.MultisampleEnable = false;
				rasterDesc.ScissorEnable = false;
				rasterDesc.SlopeScaledDepthBias = 0.0f;
				if (rs == RasterizerState::SolidAndCutBack)
				{
					XGF_Error_Check(mD3dDevice->CreateRasterizerState(&rasterDesc, &mRasterizerState[(int)rs]), "CreateRasterizerState Error");
					return mRasterizerState[(int)rs];
				}
				if (rs == RasterizerState::SolidAndCutFront)
				{
					rasterDesc.CullMode = D3D11_CULL_FRONT;
					XGF_Error_Check(mD3dDevice->CreateRasterizerState(&rasterDesc, &mRasterizerState[(int)rs]), "CreateRasterizerState Error");
					return mRasterizerState[(int)rs];
				}
				if (rs == RasterizerState::SolidAndCutNone)
				{
					rasterDesc.CullMode = D3D11_CULL_NONE;
					XGF_Error_Check(mD3dDevice->CreateRasterizerState(&rasterDesc, &mRasterizerState[(int)rs]), "CreateRasterizerState Error");
					return mRasterizerState[(int)rs];
				}
				if (rs == RasterizerState::FrameAndCutBack)
				{
					rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
					XGF_Error_Check(mD3dDevice->CreateRasterizerState(&rasterDesc, &mRasterizerState[(int)rs]), "CreateRasterizerState Error");
					return mRasterizerState[(int)rs];
				}
				if (rs == RasterizerState::FrameAndCutFront)
				{
					rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
					rasterDesc.CullMode = D3D11_CULL_FRONT;
					XGF_Error_Check(mD3dDevice->CreateRasterizerState(&rasterDesc, &mRasterizerState[(int)rs]), "CreateRasterizerState Error");
					return mRasterizerState[(int)rs];
				}
				if (rs == RasterizerState::FrameAndCutNone)
				{
					rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
					rasterDesc.CullMode = D3D11_CULL_NONE;
					XGF_Error_Check(mD3dDevice->CreateRasterizerState(&rasterDesc, &mRasterizerState[(int)rs]), "CreateRasterizerState Error");
					return mRasterizerState[(int)rs];
				}
				XGF_ReportWarn0("RasterizerState is incorrect");
				return mRasterizerState[0];

			}
			else XGF_ReportWarn0("RasterizerState is a invalid value!");

		}
		return mRasterizerState[(int)rs];
	}



	void GDI::PushRTTLayer(RenderToTexture * rtt)
	{
		mRTTs.push(rtt);
	}

	void GDI::PopRTTLayer()
	{
		mRTTs.pop();
	}

	void GDI::DrawRTT()
	{
		if (!mRTTs.empty())
		{
			ID3D11ShaderResourceView *const pSRV[1] = { nullptr };
			//TODO:Bug:1000
			mDeviceContext->PSSetShaderResources(0, 1, pSRV);
			mRTTs.top()->SetRenderTarget();
		}
		else
		{
			ID3D11ShaderResourceView *const pSRV[1] = { nullptr };
			//TODO:Bug:1000
			mDeviceContext->PSSetShaderResources(0, 1, pSRV);
			SetRenderTargetView();
		}

	}

	D3D_FEATURE_LEVEL GDI::CheckFeatureLevel()
	{
		return mFeatureLevel;
	}

	void GDI::SetBlendState(BlendState bb)
	{
		float factor[] = { 0,0,0,0,0,0 };
		ID3D11BlendState *bs = GetRawBlendState(bb);
		mDeviceContext->OMSetBlendState(bs, factor, 0xffffffff);
	}

	void GDI::SetDepthStencilState(DepthStencilState ds)
	{
		auto dds = GetRawDepthStencilState(ds);
		mDeviceContext->OMSetDepthStencilState(dds, 0);
	}

	void GDI::SetRasterizerState(RasterizerState rs)
	{
		auto rss = GetRasterizerState(rs);
		mDeviceContext->RSSetState(rss);
	}

	void GDI::CreateSwapChain()
	{
		if (mFactory2) {
			DXGI_SWAP_CHAIN_DESC1 sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.Width = mWidth;
			sd.Height = mHeight;
			sd.Format = mDisplayFormat;
			sd.Stereo = FALSE;
			sd.Scaling = DXGI_SCALING_STRETCH;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.BufferCount = 1;
			sd.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			XGF_Error_Check(mFactory2->CreateSwapChainForHwnd(mD3dDevice, mHwnd, &sd, NULL, NULL, &mSwapChain1), "CreateSwapChain Error?? ");
			PutDebugString(mSwapChain1);

			XGF_Error_Check(mSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), (void**)(&mSwapChain)), "Find SwapChain Failed!");
			PutDebugString(mSwapChain);

			XGF_ReportDebug0("The swapChain1 and swapChain are created from IDXGIFactory2");
		}
		else { // CreateSwapChainFromFactory1
			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.OutputWindow = mHwnd;
			sd.Windowed = TRUE;
			sd.BufferDesc.Format = mDisplayFormat;

			sd.BufferDesc.Width = mWidth;
			sd.BufferDesc.Height = mHeight;
			sd.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_STRETCHED;
			sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.BufferDesc.RefreshRate.Numerator = 60;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.BufferCount = 1;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			XGF_Error_Check(mFactory1->CreateSwapChain(mD3dDevice, &sd, &mSwapChain), "CreateSwapChain Error?? ");
			PutDebugString(mSwapChain);

			XGF_ReportDebug0("A swapChain is created from IDXGIFactory1");
		}
		
		
	}

}

