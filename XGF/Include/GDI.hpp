#pragma once
#include "Defines.hpp"
#include <Windows.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#include <d3dcompiler.h>
#include <vector> 
#include "RenderToTexture.hpp"
#include <stack>

namespace XGF
{
	enum class DisplayMode
	{
		Windowed,
		Borderless,
		FullScreen
	};
	enum class BlendState
	{
		NoneBlend = 0,
		AddOneOneAdd,
		AddZeroOneAdd,

		InvalidValue
	};
	enum class SamplerState
	{
		LineWrap = 0,
		LineClamp,
		LineMirror,
		LineMirrorOnce,


		InvalidValue
	};
	enum class DepthStencilState
	{
		DepthEnable = 0,
		DepthDisable,
		DepthEnableWithLessEqual,

		InvalidValue
	};
	enum class RasterizerState
	{
		SolidAndCutBack,
		SolidAndCutFront,
		SolidAndCutNone,
		FrameAndCutBack,
		FrameAndCutFront,
		FrameAndCutNone,
		InvalidValue
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
		void Destroy();
		void Clear(const float color[]);
		void Clear(const SM::Color & c);
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


		int GetWidth() { return mWidth; }
		int GetHeight() { return mHeight; }
		HWND GetHwnd();
		HWND GetTopHwnd();
		HINSTANCE GetInstance();
		ID3D11DepthStencilView * GetDepthStencilView() { return mDepthStencilView; }
		//恢复RenderTargetView
		void SetRenderTargetView();

		bool IsFullScreen() { return mDisplayMode == DisplayMode::FullScreen; }
		bool SetDisplayMode(DisplayMode dm, int left, int top, int cx, int cy, bool move, bool isClientSize = false);
		DisplayMode GetDisplayMode() { return  mDisplayMode; };
		//获取displaymode list
		//返回值：DisplayMode数目
		int GetFullScreenDisplayModes(DXGI_MODE_DESC ** c) { *c = mScreenMode[0].second; return mScreenMode[0].first; }
		void CheckFullScreenForce(bool isforce);
		ID3D11BlendState * GetRawBlendState(BlendState bs);
		ID3D11SamplerState * GetRawSamplerState(SamplerState ss);
		ID3D11DepthStencilState * GetRawDepthStencilState(DepthStencilState ds);
		ID3D11RasterizerState * GetRasterizerState(RasterizerState rs);
		void PushRTTLayer(RenderToTexture *rtt);
		void PopRTTLayer();
		void DrawRTT();
		D3D_FEATURE_LEVEL CheckFeatureLevel();
		void SetBlendState(BlendState bs);
		void SetDepthStencilState(DepthStencilState ds);
		void SetRasterizerState(RasterizerState rs);
		void CreateSwapChain();
	protected:
		IDXGIFactory2 * mFactory2 = nullptr;
		IDXGIFactory1 * mFactory1 = nullptr;

		ID3D11Device        *mD3dDevice = nullptr;
		ID3D11DeviceContext *mDeviceContext = nullptr;

		IDXGISwapChain1 * mSwapChain1 = nullptr;
		IDXGISwapChain * mSwapChain = nullptr;

		ID3D11RenderTargetView * mRenderTargetView;

		ID3D11DepthStencilView * mDepthStencilView;
		ID3D11Texture2D * mDepthStencilBuffer;

		ID3D11BlendState * mBlendState[(int)BlendState::InvalidValue];

		D3D_FEATURE_LEVEL mFeatureLevel;

		ID3D11SamplerState * mSamplerState[(int)SamplerState::InvalidValue];

		ID3D11DepthStencilState * mDepthStencilState[(int)DepthStencilState::InvalidValue];

		ID3D11RasterizerState * mRasterizerState[(int)RasterizerState::InvalidValue];

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

		std::vector<IDXGIOutput *> mOutputs;

		std::vector<IDXGIAdapter1 *> mAdapters;
		void SaveDisplayMode(int c, IDXGIOutput * pDXGIOutput);
		DXGI_FORMAT mDisplayFormat;
		RECT mLastWinRc;
		DisplayMode mLastMode;

		SIZE mFullScreenSize;
		//DXGI_ADAPTER_DESC * mAdapters;
	private:
#ifdef _DEBUG
		void QueryDebugInterface();
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

