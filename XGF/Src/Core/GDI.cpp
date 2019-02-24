#include "../../Include/GDI.hpp"
#include "../../Include/Logger.hpp"
#include <DirectXMath.h>
#include "../../Include/Context.hpp"

namespace XGF
{

	void GDI::Create()
	{
		mDisplayMode = DisplayMode::Windowed;
		mDisplayFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		XGF_Error_Check(Framework, CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&mFactory1)), "CreateDXGIFactory Failed!");
		XGF_Info_Check(Framework, mFactory1->QueryInterface(__uuidof(IDXGIFactory2), (void**)(&mFactory2)), "Find DXGIFactory2 Failed! Use DXGIFactory1 Now!");

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
				XGF_Debug(Framework, "Adapter List", "Adpapter:", i, ",DeviceID:", dc.DeviceId, ",DedicatedSystemMemory:", (dc.DedicatedSystemMemory >> 20), "MB,DedicatedVideoMemory", (dc.DedicatedVideoMemory >> 20)
					, "MB,SharedSystemMemory:", dc.SharedSystemMemory >> 20, "MB,AdapterLuid:", dc.AdapterLuid.LowPart, ",Revision:", dc.Revision, ",VendorId:", dc.VendorId
					, ",SubSysId:", dc.SubSysId, ",Description:", Logger::WCharToChar((wchar_t *)dc.Description))
















					;

				mAdapters.push_back(dxgi_adapter);

				IDXGIOutput * output;
				DXGI_OUTPUT_DESC desc;
				j = 0;
				while (dxgi_adapter->EnumOutputs(j++, &output) != DXGI_ERROR_NOT_FOUND)
				{
					output->GetDesc(&desc);
					XGF_Debug(Framework, "OutPut:", j, ",name:", Logger::WCharToChar(desc.DeviceName));
					mOutputs.push_back(output);
					SaveDisplayMode(j, output);
				}
			}
			++i;
		}
		if (mAdapters.empty())
		{
			XGF_Error(Framework, "None Adapters");
		}
		XGF_Warn_Check(Framework, mFactory1->MakeWindowAssociation(mTopHwnd, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES), "MakeWindowAssociation Error");
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
			XGF_Error_Check(Framework, D3D11CreateDevice(mAdapters[0], D3D_DRIVER_TYPE_UNKNOWN, 0, flag, featureLevels, sizeof(featureLevels) / sizeof(featureLevels[0]), D3D11_SDK_VERSION, &mD3dDevice, &curLevel, &mDeviceContext)
				, "CreateDevice Error");
		}
		else
		{
			XGF_Debug(Framework, "We can use d3d11.1");
		}
		XGF_Warn_Check(Framework, mD3dDevice->CheckMultisampleQualityLevels(
			mDisplayFormat, 4, &m4xMsaaQuality), "CheckMultisampleQualityLevels Warn");

#ifdef _DEBUG
		char const *levelstr[] = { "11","10.1","10" };
		for (size_t i = 0; i < sizeof(featureLevels) / sizeof(featureLevels[0]); i++)
		{
			if (curLevel == featureLevels[i])
			{
				XGF_Debug(Framework, "D3D Level:", levelstr[i]);
				break;
			}
		}
#endif
		mFeatureLevel = curLevel;
		XGF_Debug(Framework, "4xMsaaQuality:", m4xMsaaQuality);
		PutDebugString(mDeviceContext);
		PutDebugString(mD3dDevice);
		CreateSwapChain();

		SizeChanged(mWidth, mHeight);
	}

	void GDI::SaveDisplayMode(int c, IDXGIOutput * pDXGIOutput)
	{
		UINT  num = 0;
		pDXGIOutput->GetDisplayModeList(mDisplayFormat, 0, &num, 0);
		DXGI_MODE_DESC * pdsk = new DXGI_MODE_DESC[num];
		mScreenMode.push_back(std::make_pair(num, pdsk));
		pDXGIOutput->GetDisplayModeList(mDisplayFormat, 0, &num, pdsk);

		XGF_Debug(Framework, "DisplayMode has ", num);
	}
	void GDI::Destroy()
	{
		mSwapChain->SetFullscreenState(false, nullptr);
		mDisplayFrameBuffer.Shutdown();

		for(auto & it : mRasterizerStates.GetMap())
		{
			it.second.GetPtr<ID3D11RasterizerState>()->Release();
		}
		mRasterizerStates.GetMap().clear();

		for (auto & it : mDepthStencilStates.GetMap())
		{
			it.second.GetPtr<ID3D11DepthStencilState>()->Release();
		}
		mDepthStencilStates.GetMap().clear();

		for (auto & it : mBlendStates.GetMap())
		{
			it.second.GetPtr<ID3D11BlendState>()->Release();
		}
		mBlendStates.GetMap().clear();

		for (auto & it : mSamplerStates.GetMap())
		{
			it.second.GetPtr<ID3D11SamplerState>()->Release();
		}
		mSamplerStates.GetMap().clear();

		for (auto & it : mAdapters)
		{
			it->Release();
		}
		for (auto & it : mOutputs)
		{
			it->Release();
		}
		for (auto & it :  mScreenMode)
		{
			delete[] it.second;
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
			if (hr == DXGI_STATUS_OCCLUDED)
			{
				mIsStandby = true;
				XGF_Debug(Framework, "ERROR IN Present! ", "DXGI_STATUS_OCCLUDED");
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
		XGF_Debug(Framework, "SIZE Changed:", ClientWidth, ",", ClientHeight);
		if (ClientWidth < 1)
			ClientWidth = 1;
		if (ClientHeight < 1)
			ClientHeight = 1;

		mDisplayFrameBuffer.Shutdown();
		XGF_Error_Check(Framework, mSwapChain->ResizeBuffers(1, ClientWidth, ClientHeight, mDisplayFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH), "ResizeBuffers Error");
		PutDebugString(mSwapChain);
		ID3D11Texture2D* backBufferPtr;
		XGF_Error_Check(Framework, mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr), "GetSwapChainBuffer is Error");

		
		mDisplayFrameBuffer.Initialize(backBufferPtr, DepthStencilFormat::Depth24FloatStencil8Uint);
		backBufferPtr->Release();
		backBufferPtr = 0;

		mDisplayFrameBuffer.Bind();

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
			XGF_Info(Framework, "full screen");
			XGF_Warn_Check(Framework, mSwapChain->SetFullscreenState(isFullscreen, 0), "Can't switch fullscreenState");
			XGF_Warn_Check(Framework, mSwapChain->ResizeTarget(&dxm), "ResizeTarget Error");
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

	GpuBuffer GDI::CreateIndexBuffer(unsigned len, GpuBufferType type, void* dataAddress)
	{
		auto *indexList = new Index[len];
		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_DYNAMIC;
		ibd.ByteWidth = sizeof(Index) *(len);
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		D3D11_SUBRESOURCE_DATA iinitData;
		ZeroMemory(&iinitData, sizeof(iinitData));
		iinitData.pSysMem = indexList;
		ID3D11Buffer * indexBuffer;
		XGF_Error_Check(Render, mD3dDevice->CreateBuffer(&ibd, &iinitData, &indexBuffer), "CreateIndexBuffer Error");
		//mIndexBuffer = CreateBuffer(mGDI, D3D11_BIND_INDEX_BUFFER, 0, sizeof(index) *(mMaxIndexCount * mMaxPreRenderFrameCount), 0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		PutDebugString(indexBuffer);
		delete[] indexList;
		return GpuBuffer(indexBuffer, len);
	}

	GpuBuffer GDI::CreateVertexBuffer(unsigned len, GpuBufferType type, void* dataAddress)
	{
		ID3D11Buffer * buffer;
		D3D11_BUFFER_DESC vertexDesc;
		char * data = (char *)dataAddress;
		if (!dataAddress)
			data = new char[len];
		ZeroMemory(&vertexDesc, sizeof(vertexDesc));
		vertexDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = len;
		vertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		D3D11_SUBRESOURCE_DATA resourceData;
		ZeroMemory(&resourceData, sizeof(resourceData));
		resourceData.pSysMem = data;
		XGF_Error_Check(Render, mD3dDevice->CreateBuffer(&vertexDesc, &resourceData, &buffer), "CreateVertexBuffer Error");
		PutDebugString((buffer));
		if (!dataAddress)
			delete[] data;
		return GpuBuffer(buffer, len);
	}

	GpuBuffer GDI::CreateConstantBuffer(unsigned size)
	{
		XGF_ASSERT(size % 4 == 0);

		ID3D11Buffer * buffer;
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = size;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		XGF_Error_Check(Render, mD3dDevice->CreateBuffer(&desc, 0, &buffer), "CreateVertexBuffer Error");
		PutDebugString((buffer));
		return GpuBuffer(buffer, size);
	}

	void* GDI::Map(GpuBuffer& gpuBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		mDeviceContext->Map(gpuBuffer.buffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource);
		return mappedResource.pData;
	}

	void GDI::UnMap(GpuBuffer & gpuBuffer)
	{
		mDeviceContext->Unmap(gpuBuffer.buffer, 0);
	}

	void GDI::CopyToBuffer(GpuBuffer& buffer, void* src)
	{
		auto * d = Map(buffer);
		memcpy(d, src, buffer.size);
		UnMap(buffer);
	}
#ifdef _DEBUG
	void GDI::QueryDebugInterface()
	{
		ID3D11Debug *d3dDebug;
		mD3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
		d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		d3dDebug->Release();
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

	ID3D11ShaderResourceView * GDI::CreateRenderableTexture(unsigned width, unsigned height, DXGI_FORMAT format, char * ptrContent)
	{
		char * ptr = ptrContent;
		if (!ptr)
		{
			ptr = new char[width * height];
			memset(ptr, 0, width * height * sizeof(char));
		}
		ID3D11Texture2D * texutre;
		D3D11_SUBRESOURCE_DATA __subData;
		__subData.pSysMem = ptr;
		__subData.SysMemPitch = width;
		__subData.SysMemSlicePitch = width * height;
		D3D11_TEXTURE2D_DESC  texture2d;

		texture2d.Width = width;
		texture2d.Height = height;
		texture2d.MipLevels = 1;
		texture2d.ArraySize = 1;

		texture2d.SampleDesc.Count = 1;
		texture2d.SampleDesc.Quality = 0;
		texture2d.Usage = D3D11_USAGE_DEFAULT;
		texture2d.Format = format;
		texture2d.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		texture2d.CPUAccessFlags = 0;
		texture2d.MiscFlags = 0;

		XGF_Error_Check(Application, GetDevice()->CreateTexture2D(&texture2d, &__subData, &texutre), "CreateTexture2D at font class failed");

		PutDebugString(texutre);
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texture2d.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipLevels = texture2d.MipLevels;
		viewDesc.Texture2D.MostDetailedMip = 0;
		ID3D11ShaderResourceView * srv;
		XGF_Error_Check(Application, GetDevice()->CreateShaderResourceView(texutre, &viewDesc, &srv), "Create font SRV failed");
		PutDebugString(srv);
		if (!ptrContent)
		{
			delete[] ptr;
		}
		texutre->Release();
		return srv;
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
	void CopyBlendStateToDesc(D3D11_RENDER_TARGET_BLEND_DESC & desc, const BlendStateTarget & target)
	{
		desc.BlendEnable = target.GetBlendEnable();
		desc.BlendOp = (D3D11_BLEND_OP) target.GetBlendOp();
		desc.BlendOpAlpha = (D3D11_BLEND_OP)target.GetBlendOpAlpha();
		desc.DestBlend = (D3D11_BLEND)target.GetDestBlend();
		desc.DestBlendAlpha = (D3D11_BLEND)target.GetDestBlendAlpha();
		desc.RenderTargetWriteMask = target.GetRenderTargetWriteMask();
		desc.SrcBlend = (D3D11_BLEND)target.GetSrcBlend();
		desc.SrcBlendAlpha = (D3D11_BLEND)target.GetSrcBlendAlpha();
	}

	RawBlendState GDI::CreateBlendState(const BlendState& bs)
	{
		D3D11_BLEND_DESC blendDesc;
		blendDesc.AlphaToCoverageEnable = bs.GetAlphaToCoverageEnable();
		blendDesc.IndependentBlendEnable = bs.GetIndependentBlendEnable();
		CopyBlendStateToDesc(blendDesc.RenderTarget[0], bs.GetRenderTarget(0));
		if(bs.GetIndependentBlendEnable())
		{
			const int c = bs.GetRenderTargetCount();
			for(int i = 1; i < c; i++)
			{
				CopyBlendStateToDesc(blendDesc.RenderTarget[i], bs.GetRenderTarget(i));
			}
		}
		ID3D11BlendState * state;
		mD3dDevice->CreateBlendState(&blendDesc, &state);
		PutDebugString(state);
		return RawBlendState(state);
	}

	RawBlendState GDI::GetBlendState(const BlendState& bs)
	{
		auto s = mBlendStates.GetRawState(bs);
		if (s.has_value())
		{
			return s.value();
		}
		auto rawBS = CreateBlendState(bs);
		mBlendStates.SaveRawState(bs, rawBS);
		return rawBS;
	}

	RawDepthStencilState GDI::CreateDepthStencilState(const DepthStencilState& ds)
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = ds.GetDepthEnable();
		depthStencilDesc.DepthWriteMask = (D3D11_DEPTH_WRITE_MASK) ds.GetDepthWriteMask();//D3D11_DEPTH_WRITE_MASK_ZERO禁止写深度缓冲 
		depthStencilDesc.DepthFunc = (D3D11_COMPARISON_FUNC) ds.GetDepthFunc();
		depthStencilDesc.StencilEnable = ds.GetStencilEnable();
		depthStencilDesc.StencilReadMask = ds.GetStencilReadMask();
		depthStencilDesc.StencilWriteMask = ds.GetStencilWriteMask();
		// 对于front face 像素使用的模版操作操作. 
		depthStencilDesc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)ds.GetFrontFaceStencilFailOp();
		depthStencilDesc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP) ds.GetFrontFaceStencilDepthFailOp();
		depthStencilDesc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)ds.GetFrontFaceStencilPassOp();
		depthStencilDesc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC) ds.GetFrontFaceStencilFunc();
		// 对于back face像素使用的模版操作模式. 
		depthStencilDesc.BackFace.StencilFailOp = (D3D11_STENCIL_OP)ds.GetBackFaceStencilFailOp();
		depthStencilDesc.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)ds.GetBackFaceStencilDepthFailOp();
		depthStencilDesc.BackFace.StencilPassOp = (D3D11_STENCIL_OP)ds.GetBackFaceStencilPassOp();
		depthStencilDesc.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)ds.GetBackFaceStencilFunc();
		ID3D11DepthStencilState * state;
		XGF_Error_Check(Framework, mD3dDevice->CreateDepthStencilState(&depthStencilDesc, &state), "CreateDepthStencilState Error");
		PutDebugString(state);
		return RawDepthStencilState(state);
	}

	RawDepthStencilState GDI::GetDepthStencilState(const DepthStencilState& ds)
	{
		auto s = mDepthStencilStates.GetRawState(ds);
		if (s.has_value())
		{
			return s.value();
		}
		auto rawDS = CreateDepthStencilState(ds);
		mDepthStencilStates.SaveRawState(ds, rawDS);
		return rawDS;
	}

	RawRasterizerState GDI::CreateRasterizerState(const RasterizerState& rs)
	{
		D3D11_RASTERIZER_DESC rasterDesc;

		rasterDesc.AntialiasedLineEnable = rs.GetAntialiasedLineEnable();
		rasterDesc.CullMode = (D3D11_CULL_MODE)rs.GetCullMode();
		rasterDesc.DepthBias = rs.GetDepthBias();
		rasterDesc.DepthBiasClamp = rs.GetDepthBiasClamp();
		rasterDesc.DepthClipEnable = rs.GetDepthClipEnable();
		rasterDesc.FillMode = (D3D11_FILL_MODE)rs.GetFillMode();
		rasterDesc.FrontCounterClockwise = rs.GetFrontCounterClockwise();
		rasterDesc.MultisampleEnable = rs.GetMultisampleEnable();
		rasterDesc.ScissorEnable = rs.GetScissorEnable();
		rasterDesc.SlopeScaledDepthBias = rs.GetSlopeScaledDepthBias();
		ID3D11RasterizerState * state;
		XGF_Error_Check(Framework, mD3dDevice->CreateRasterizerState(&rasterDesc, &state), "CreateRasterizerState Error");
		PutDebugString(state);
		return RawRasterizerState(state);
	}

	RawRasterizerState GDI::GetRasterizerState(const RasterizerState& rs)
	{
		auto s = mRasterizerStates.GetRawState(rs);
		if (s.has_value())
		{
			return s.value();
		}
		auto rawRS = CreateRasterizerState(rs);
		mRasterizerStates.SaveRawState(rs, rawRS);
		return rawRS;
	}

	RawSamplerState GDI::CreateSamplerState(const SamplerState & s)
	{
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = (D3D11_FILTER)s.GetFilter();
		samplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)s.GetAddressU();
		samplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)s.GetAddressV();
		samplerDesc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)s.GetAddressW();
		samplerDesc.MipLODBias = s.GetMipLODBias();
		samplerDesc.MaxAnisotropy = s.GetMaxAnisotropy();
		samplerDesc.ComparisonFunc = (D3D11_COMPARISON_FUNC)s.GetComparisonFunc();
		samplerDesc.BorderColor[0] = s.GetBorderColor().x;
		samplerDesc.BorderColor[1] = s.GetBorderColor().y;
		samplerDesc.BorderColor[2] = s.GetBorderColor().z;
		samplerDesc.BorderColor[3] = s.GetBorderColor().w;
		samplerDesc.MinLOD = s.GetMinLOD();
		samplerDesc.MaxLOD = s.GetMaxLOD();
		ID3D11SamplerState* state;
		GetDevice()->CreateSamplerState(&samplerDesc, &state);
		PutDebugString(state);
		return RawSamplerState(state);
	}

	RawSamplerState GDI::GetSamplerState(const SamplerState & ss)
	{
		auto s = mSamplerStates.GetRawState(ss);
		if(s.has_value())
		{
			return s.value();
		}
		auto rawSS = CreateSamplerState(ss);
		mSamplerStates.SaveRawState(ss, rawSS);
		return rawSS;
	}

	D3D_FEATURE_LEVEL GDI::CheckFeatureLevel()
	{
		return mFeatureLevel;
	}

	void GDI::SetBlendState(RawBlendState bb)
	{
		float factor[] = { 0,0,0,0,0,0 };
		mDeviceContext->OMSetBlendState(bb.GetPtr<ID3D11BlendState>(), factor, 0xffffffff);
	}

	void GDI::SetDepthStencilState(RawDepthStencilState ds)
	{
		mDeviceContext->OMSetDepthStencilState(ds.GetPtr<ID3D11DepthStencilState>(), 0);
	}

	void GDI::SetRasterizerState(RawRasterizerState rs)
	{
		mDeviceContext->RSSetState(rs.GetPtr<ID3D11RasterizerState>());
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
			if (mEnable4xMsaa && m4xMsaaQuality > 0)
			{
				sd.SampleDesc.Count = 4;
				sd.SampleDesc.Quality = m4xMsaaQuality - 1;
			}
			else
			{
				sd.SampleDesc.Count = 1;
				sd.SampleDesc.Quality = 0;
			}
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.BufferCount = 1;
			sd.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			XGF_Error_Check(Framework, mFactory2->CreateSwapChainForHwnd(mD3dDevice, mHwnd, &sd, NULL, NULL, &mSwapChain1), "CreateSwapChain Error?? ");
			PutDebugString(mSwapChain1);

			XGF_Error_Check(Framework, mSwapChain1->QueryInterface(__uuidof(IDXGISwapChain), (void**)(&mSwapChain)), "Find SwapChain Failed!");
			PutDebugString(mSwapChain);

			XGF_Debug(Framework, "The swapChain1 and swapChain are created from IDXGIFactory2");
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
			if (mEnable4xMsaa && m4xMsaaQuality > 0)
			{
				sd.SampleDesc.Count = 4;
				sd.SampleDesc.Quality = m4xMsaaQuality - 1;
			}
			else
			{
				sd.SampleDesc.Count = 1;
				sd.SampleDesc.Quality = 0;
			}
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.BufferCount = 1;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			XGF_Error_Check(Framework, mFactory1->CreateSwapChain(mD3dDevice, &sd, &mSwapChain), "CreateSwapChain Error?? ");
			PutDebugString(mSwapChain);

			XGF_Debug(Framework, "A swapChain is created from IDXGIFactory1");
		}


	}

	void GDI::ReCreateSwapChain()
	{
		BOOL bl;
		IDXGIOutput * output;
		mSwapChain->GetFullscreenState(&bl, &output);
		mSwapChain->SetFullscreenState(false, nullptr);
		if (mSwapChain1)
		{
			mSwapChain1->Release();
		}
		if (mSwapChain)
		{
			mSwapChain->Release();
		}
		CreateSwapChain();
		// TODO: 全屏相关逻辑
		SetFullScreen(bl, 0);
	}


	void GDI::Able4xMsaa()
	{
		if (!mEnable4xMsaa)
		{
			mEnable4xMsaa = true;
			ReCreateSwapChain();
		}
	}

	void GDI::Disable4xMsaa()
	{
		if (mEnable4xMsaa)
		{
			mEnable4xMsaa = false;
			ReCreateSwapChain();
		}
	}

	bool GDI::IsEnable4xMsaa() const
	{
		return mEnable4xMsaa;
	}

	int GDI::Query4xMsaaQuality() const
	{
		return m4xMsaaQuality;
	}

	bool GDI::CanEnable4xMsaa() const
	{
		return m4xMsaaQuality > 0;
	}

	bool GDI::IsDisplayMode(DisplayMode displayMode) const
	{
		return mDisplayMode == displayMode;
	}
}

