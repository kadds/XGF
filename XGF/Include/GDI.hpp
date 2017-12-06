#pragma once
#include "Defines.hpp"
#include <Windows.h>
#include <d3d11_1.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#include <d3dcompiler.h>
#include <vector> 
#include "RenderToTexture.hpp"
#include <stack>

namespace XGF
{
	enum DisplayMode
	{
		Windowed,
		Borderless,
		FullScreen
	};
	enum BlendState
	{
		NoneBlend,
		AddOneOneAdd,
		AddZeroOneAdd
	};
	/*
	底层图形接口
	调整窗口尺寸可使用ResizeTarget函数
	*/
	class GDI
	{
	public:
		explicit GDI() {};
		~GDI() {};
		//框架调用
		void Create();
		//框架调用
		void Destory();
		void Clear(float color[]);
		void Clear(Color & c);
		void ClearDepthStencilBuffer();
		void Present(bool isVsync);
		//初始化函数，必须是该类第一个调用的函数
		//用户调用
		void Initialize(HINSTANCE instance, HWND WndHwnd, HWND TopHwnd, UINT ClientWidth, UINT ClientHeight);
		//框架调用
		void SizeChanged(UINT ClientWidth, UINT ClientHeight);

		void ResizeTarget(UINT x, UINT y);
		void SetFullScreen(bool isFullscreen, int pos);

		ID3D11Device * GetDevice() { return mD3dDevice; }
		ID3D11DeviceContext * GetDeviceContext() { return mDeviceContext; }

		void SetFillMode(bool isSold);
		void SetZBufferMode(bool isOpenZBuffer);
		int GetWidth() { return mWidth; }
		int GetHeight() { return mHeight; }
		HWND GetHwnd();
		HWND GetTopHwnd();
		HINSTANCE GetInstance();
		ID3D11DepthStencilView * GetDepthStencilView() { return mDepthStencilView; }
		//恢复RenderTargetView
		void SetRenderTargetView();

		bool IsFullScreen() { return mDisplayMode == FullScreen; }
		bool SetDisplayMode(DisplayMode dm, int left, int top, int cx, int cy, bool move, bool isClientSize = false);
		DisplayMode GetDisplayMode() { return  mDisplayMode; };
		//获取displaymode list
		//返回值：mode数目
		int GetFullScreenDisplayModes(DXGI_MODE_DESC ** c) { *c = mScreenMode[0].second; return mScreenMode[0].first; }
		void CheckFullScreenForce(bool isforce);

		void SetDefaultSamplerState();
		void CloseSamplerState();
		void PushRTTLayer(RenderToTexture *rtt);
		void PopRTTLayer();
		void DrawRTT();
		D3D_FEATURE_LEVEL CheckFeatureLevel();
		void SetBlendState(BlendState);
	protected:
		ID3D11Device        *mD3dDevice;
		ID3D11DeviceContext *mDeviceContext;
		IDXGISwapChain * mSwapChain;
		ID3D11RenderTargetView * mRenderTargetView;

		ID3D11DepthStencilView * mDepthStencilView;
		ID3D11Texture2D * mDepthStencilBuffer;

		ID3D11BlendState * mDisableBlendState;
		ID3D11BlendState * mBlendState;
		ID3D11BlendState * mCEVBlendState;

		D3D_FEATURE_LEVEL mFeatureLevel;

		ID3D11SamplerState * mLineSamplerState;
		//开启 Z 深度 缓冲state
		ID3D11DepthStencilState * mDepthStencilState;
		//禁止 Z 深度 缓冲state
		ID3D11DepthStencilState * md3dDisableDepthStencilState;

		ID3D11DepthStencilState * mDepthStencilStateLessEqual;

		//普通模式渲染的RasterState
		ID3D11RasterizerState * mRasterState;
		//线框模式渲染的RasterState
		ID3D11RasterizerState * mFrameRasterState;
		bool mIsOpenFillSold = true;
		bool mIsOpenZBuffer = true;
		bool mIsStandby = false;
		//显示的窗口句柄
		HWND mHwnd;
		//顶级窗口句柄
		HWND mTopHwnd;

		UINT mWidth;
		UINT mHeight;
		HINSTANCE mInstance;
		std::stack<RenderToTexture *> mRTTs;
		DisplayMode mDisplayMode;
		std::vector<std::pair<int, DXGI_MODE_DESC *>> mScreenMode;
		int mNowInDisplayMode;
		std::vector<IDXGIOutput *> mOutputs;
		std::vector<IDXGIAdapter *> mAdapters;
		void SaveDisplayMode(int c, IDXGIOutput * pDXGIOutput);
		DXGI_FORMAT mDisplayFormat;
		RECT mLastWinRc;
		DisplayMode mLastMode;

		SIZE mFullScreenSize;
		//DXGI_ADAPTER_DESC * mAdapters;
	private:
#ifdef _DEBUG
		void QueryInterface();
#endif
	private:
		DISALLOW_COPY_AND_ASSIGN(GDI);
	};
#ifdef _DEBUG
#define PutDebugString(de) de->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(_FUNNAME_XGF_), _FUNNAME_XGF_);
#else
#define PutDebugString(de) ;
#endif
}

