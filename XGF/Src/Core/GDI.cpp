#include "../../Include/GDI.hpp"
#include "../../Include/Log.hpp"
#include <DirectXMath.h>

void GDI::Create()
{
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,};
	D3D_FEATURE_LEVEL   curLevel;
	
	//IDXGIFactory::MakeWindowAssociation ();
	//交换链描述
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	//后台缓冲区描述
	//宽度、高度
	sd.BufferDesc.Width = mWidth;
	sd.BufferDesc.Height = mHeight;
	//像素格式
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//刷新率
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	//扫描方式
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//按比例伸缩
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//多重采样描述
	//每像素多重采样个数
	sd.SampleDesc.Count = 1;
	//图像质量等级，可选范围为0到ID3D11Device::CheckMultisampleQualityLevels
	sd.SampleDesc.Quality = 0;
	//CPU访问后台缓冲区的选项
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//后台缓冲区数量
	sd.BufferCount = 1;
	//进行渲染的窗口句柄
	sd.OutputWindow = mHwnd;
	//窗口/全屏显示
	sd.Windowed = TRUE;
	//将后台缓冲区内容复制到前台缓冲区的方式
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//交换链行为
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	int flag = 0;
#ifdef _DEBUG
	flag = D3D11_CREATE_DEVICE_DEBUG,
#endif

	Check(D3D11CreateDevice(
		0,                  //默认适配器  
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		flag,
		featureLevels, 4,
		D3D11_SDK_VERSION,
		&mD3dDevice,
		&curLevel,
		&mDeviceContext));
	UINT  m4xMsaaQuality;
	Check(mD3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	sd.SampleDesc.Quality = m4xMsaaQuality;
	OutputDebugStringEx(L"Level:%d\n",curLevel);
	OutputDebugStringEx(L"m4xMsaaQuality:%d\n", m4xMsaaQuality);

	IDXGIDevice * pDXGIDevice = nullptr;
	mD3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
	IDXGIAdapter * pDXGIAdapter = nullptr;
	pDXGIDevice->GetAdapter(&pDXGIAdapter);
	IDXGIFactory * pIDXGIFactory = nullptr;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);
	pIDXGIFactory->CreateSwapChain(mD3dDevice, &sd, &mSwapChain);
	pIDXGIFactory->MakeWindowAssociation(mHwnd, DXGI_MWA_NO_ALT_ENTER);

	IDXGIAdapter * eDXGIAdapter = nullptr;
	int i = 0;
	while (pIDXGIFactory->EnumAdapters(i, &eDXGIAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC dad;
		eDXGIAdapter->GetDesc(&dad);
		OutputDebugStringEx(L"显卡%d: DeviceId:%d,SharedSystemMemory:%dMB,DedicatedSystemMemory:%dMB,DedicatedVideoMemory:%dMB,AdapterLuid:%d,Description:%s\n"
			,i, dad.DeviceId, dad.SharedSystemMemory >> 20, dad.DedicatedSystemMemory >> 20, dad.DedicatedVideoMemory >> 20, dad.AdapterLuid,dad.Description);
		mAdapters.push_back(eDXGIAdapter);
		++i;
	}
	IDXGIOutput * output;
	i = 0;
	while (pDXGIAdapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		OutputDebugStringEx(L"OutPut:%d\n"
			, i);

		mOutputs.push_back(output);
		SaveDisplayMode(i, output);
		++i;
	}
	if (i == 0)
		ReportError("Can't Get IDXGIOutput!");
	pDXGIDevice->Release();
	pDXGIAdapter->Release();
	pIDXGIFactory->Release();
	PutDebugString(mDeviceContext);
	PutDebugString(mD3dDevice);
	PutDebugString(mSwapChain);
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	// 初始化深度模版状态描述. 
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	// 设置深度模版状态描述. 
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
	// 创建深度模版状态，使其生效 
	Check(mD3dDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));
	// 设置深度模版状态. 
	depthStencilDesc.DepthEnable = false;
	Check(mD3dDevice->CreateDepthStencilState(&depthStencilDesc, &md3dDisableDepthStencilState));

	PutDebugString(mDepthStencilState);
	PutDebugString(md3dDisableDepthStencilState);

	SetZBufferMode(mIsOpenZBuffer);

	D3D11_RASTERIZER_DESC rasterDesc;
	// 设置光栅化描述，指定多边形如何被渲染. 
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
	// 创建sold光栅化状态 
	Check(mD3dDevice->CreateRasterizerState(&rasterDesc, &mRasterState));
	//创建Frame光栅化状态
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME; 
	Check(mD3dDevice->CreateRasterizerState(&rasterDesc, &mFrameRasterState));
	PutDebugString(mRasterState);
	PutDebugString(mFrameRasterState);

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
	//创建ID3D11BlendState接口  
	mD3dDevice->CreateBlendState(&blendDesc, &mBlendState);
	PutDebugString(mBlendState);
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	mD3dDevice->CreateBlendState(&blendDesc, &mDisableBlendState);
	PutDebugString(mDisableBlendState);
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  //线性插值(三种方式,点过滤,线性过滤,异性过滤)
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
	//gdi->setFillMode(false);
	mD3dDevice->CreateSamplerState(&samplerDesc, &mLineSamplerState);
	PutDebugString(mLineSamplerState);
	mNowInDisplayMode = 0;
	SetFillMode(mIsOpenFillSold);
	SizeChanged(mWidth, mHeight);
}
void GDI::SaveDisplayMode(int c, IDXGIOutput * pDXGIOutput)
{
	UINT  num = 0;
	pDXGIOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num, 0);
	DXGI_MODE_DESC * pdsk = new DXGI_MODE_DESC[num];
	mDisplayMode.push_back(std::make_pair(num, pdsk));
	pDXGIOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num, pdsk);

#ifdef _DEBUG
	for (UINT i = 0; i < num; i++)
	{
		OutputDebugStringEx(L"DisplayMode%d: Width:%d,Height:%d,RefreshRate[Denominator]:%d,RefreshRate[Numerator]:%d"
			",Scaling:%d,ScanlineOrdering:%d,Format:%d\n"
			, i, pdsk[i].Width, pdsk[i].Height, pdsk[i].RefreshRate.Denominator, pdsk[i].RefreshRate.Numerator
			, pdsk[i].Scaling, pdsk[i].ScanlineOrdering, pdsk[i].Format);

	}
#endif
}
void GDI::Destory()
{
	if(mSwapChain)
		mSwapChain->SetFullscreenState(false, nullptr);
	if (mLineSamplerState)
		mLineSamplerState->Release();
	mLineSamplerState = nullptr;
	if (mBlendState)
		mBlendState->Release();
	mBlendState = nullptr;
	if (mDisableBlendState)
		mDisableBlendState->Release();
	mDisableBlendState = nullptr;
	if (mDepthStencilView)
		mDepthStencilView->Release();
	mDepthStencilView = nullptr;
	if(mDepthStencilBuffer)
		mDepthStencilBuffer->Release();
	mDepthStencilBuffer = nullptr;
	if (mRenderTargetView)
		mRenderTargetView->Release();
	mRenderTargetView = nullptr;
	if (mRasterState)
		mRasterState->Release();
	mRasterState = nullptr;
	if (mFrameRasterState)
		mFrameRasterState->Release();
	mFrameRasterState = nullptr;
	if(md3dDisableDepthStencilState)
		md3dDisableDepthStencilState->Release();
	md3dDisableDepthStencilState = nullptr;
	if(mDepthStencilState)
		mDepthStencilState->Release();
	mDepthStencilState = nullptr;
	for each (auto var in mAdapters)
	{
		var->Release();
	}
	for each (auto var in mOutputs)
	{
		var->Release();
	}
	for each (auto var in mDisplayMode)
	{
		delete[] var.second;
	}
	
	if (mSwapChain)
		mSwapChain->Release();
	mSwapChain = nullptr;
	if(mDeviceContext)
		mDeviceContext->Release();
	mDeviceContext = nullptr;

#ifdef _DEBUG
	QueryInterface();
#endif();
	if(mD3dDevice)
		mD3dDevice->Release();
	mD3dDevice = nullptr;

}

void GDI::Clean(float color[])
{
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, color);
	//清除深度缓冲.  
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	ID3D11ShaderResourceView *const pSRV[1] = { nullptr };
	mDeviceContext->PSSetShaderResources(0, 1, pSRV);
}
void GDI::Clean(Color & c)
{
	float color[4];
	color[0] = c.x;
	color[1] = c.y;
	color[2] = c.z;
	color[3] = c.w;
	Clean(color);
}
void GDI::Present(bool isVsync)
{
	mSwapChain->Present(isVsync,0);
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
	//交换链为空直接返回 
	if (!mSwapChain)
		return ;

	//窗口最小化时候为0，会创建缓冲失败 
	if (ClientWidth < 1)
		ClientWidth = 1;
	if (ClientHeight < 1)
		ClientHeight = 1;

	if (mRenderTargetView)
	{
		mRenderTargetView->Release();
		mRenderTargetView = 0;
	}
	if (mDepthStencilView){
		mDepthStencilView->Release();
		mDepthStencilView = 0;
	}

	if (mDepthStencilBuffer)
	{
		mDepthStencilBuffer->Release();
		mDepthStencilBuffer = 0;
	}
	//QueryInterface();
	//改变交换链中后后缓冲大小后，重新创建渲染目标视图 
	Check(mSwapChain->ResizeBuffers(1, ClientWidth, ClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	PutDebugString(mSwapChain);
	// 得到交换链中的后缓冲指针. 
	ID3D11Texture2D* backBufferPtr;
	Check(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr));

	// 用后缓冲创建渲染目标视图. 
	Check(mD3dDevice->CreateRenderTargetView(backBufferPtr, NULL, &mRenderTargetView));
	//释放后缓冲.(引用计数减1) 
	backBufferPtr->Release();
	backBufferPtr = 0;
	PutDebugString(mRenderTargetView);

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	//创建深度模版视图 
	// 初始化深度缓冲描述. 
	// 初始化深度缓冲描述. 
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	//设置深度缓冲描述 
	depthBufferDesc.Width = ClientWidth;
	depthBufferDesc.Height = ClientHeight;
	depthBufferDesc.MipLevels = 1;  //对于深度缓冲为1 
	depthBufferDesc.ArraySize = 1;  //对于深度缓冲为1，对于纹理，这2个参数有更多用途 
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	// 创建深度缓冲. 
	Check(mD3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer));
	PutDebugString(mDepthStencilBuffer);
	// 初始化深度模版视图. 
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	// 设置深度模版视图描述. 
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	// 创建深度模版视图. 
	Check(mD3dDevice->CreateDepthStencilView(mDepthStencilBuffer,
		&depthStencilViewDesc, &mDepthStencilView));
	// 绑定渲染目标视图和深度缓冲到渲染管线. 
	PutDebugString(mDepthStencilView);
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	//设置光栅化状态，使其生效 
	SetFillMode(mIsOpenFillSold);

	D3D11_VIEWPORT view;
	view.MinDepth = 0.0f;
	view.MaxDepth = 1.0f;
	view.Width = static_cast<FLOAT>(ClientWidth);
	view.Height = static_cast<FLOAT>(ClientHeight);
	view.TopLeftX = 0;
	view.TopLeftY = 0;
	mDeviceContext->RSSetViewports(1, &view);
	mWidth = ClientWidth;
	mHeight = ClientHeight;
}

void GDI::ResizeTarget(UINT x, UINT y)
{
	DXGI_MODE_DESC mode;
	mode.Width = x;
	mode.Height = y;
	mode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	mSwapChain->ResizeTarget(&mode);
}

void GDI::SetFullScreen(bool isFullscreen)
{
	/*
	if (isFullscreen)
	{
		DEVMODE dmScrrenSettings;
		int ScrrenWidth = GetSystemMetrics(SM_CXSCREEN);
		int ScrrenHeight = GetSystemMetrics(SM_CYSCREEN);
			//如果为全屏幕,则设定屏幕为用户桌面的最大化并且为32bit
			memset(&dmScrrenSettings, 0, sizeof(dmScrrenSettings));
			dmScrrenSettings.dmSize = sizeof(dmScrrenSettings);
			dmScrrenSettings.dmPelsWidth = (unsigned long)ScrrenWidth;
			dmScrrenSettings.dmPelsHeight = (unsigned long)ScrrenHeight;
			dmScrrenSettings.dmBitsPerPel = 32;
			dmScrrenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			//改变显示设定,设置为全屏幕
			ChangeDisplaySettings(&dmScrrenSettings, CDS_FULLSCREEN);
	}
	else
		ChangeDisplaySettings(NULL, 0);
		*/
	if (isFullscreen)
	{
		mSwapChain->SetFullscreenState(isFullscreen, 0);
		mSwapChain->ResizeTarget(&(mDisplayMode.at(0).second[mNowInDisplayMode]));
		//SizeChanged(mDisplayMode.at(0)[mNowInDisplayMode].Width, mDisplayMode.at(0)[mNowInDisplayMode].Height);
	}
	else
	{
		//mSwapChain->ResizeTarget(&(mDisplayMode.at(0).second[mNowInDisplayMode]));
		mSwapChain->SetFullscreenState(isFullscreen, 0);
		
	}
}
void GDI::SetFillMode(bool isSold)
{
	mIsOpenFillSold = isSold;
	if (isSold) 
		mDeviceContext->RSSetState(mRasterState);
	else 
		mDeviceContext->RSSetState(mFrameRasterState);
}
#ifdef _DEBUG
void GDI::QueryInterface()
{
	ID3D11Debug *d3dDebug; 
	mD3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}
#endif
void GDI::SetZBufferMode(bool isOpenZBuffer) {
	mIsOpenZBuffer = isOpenZBuffer;
	if (isOpenZBuffer)
		mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);
	else
		mDeviceContext->OMSetDepthStencilState(md3dDisableDepthStencilState, 1);
}

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

void GDI::SetFullScreenDisplayMode(int pos)
{
	mNowInDisplayMode = pos;
}


void GDI::OpenDefaultBlendState()
{
	float factor[4] = { 0.f,0.f,0.f,0.f };
	mDeviceContext->OMSetBlendState(mBlendState, factor, 0xffffffff);
}

void GDI::CloseBlendState()
{
	float factor[4] = { 0.f,0.f,0.f,0.f };
	mDeviceContext->OMSetBlendState(mDisableBlendState, factor, 0xffffffff);
}

void GDI::SetDefaultSamplerState()
{
	mDeviceContext->PSSetSamplers(0, 1, &mLineSamplerState);  //S0注册 对应0
}

void GDI::CloseSamplerState()
{
	ID3D11SamplerState * const ss[]= { NULL };
	mDeviceContext->PSSetSamplers(0, 1, ss);  //S0注册 对应0
}
