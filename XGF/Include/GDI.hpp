#pragma once
#include "Defines.hpp"
#include <Windows.h>
#include <d3d11_1.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#include <d3dcompiler.h>
#include <vector> 
/*
�ײ�ͼ�νӿ�
�������ڳߴ���ʹ��ResizeTarget������������Win API
ʹ��ȫ��ģʽ��ѡ��һ��DisplayMode(DXGI_MODE)��call SetFullScreenDisplayMode��pos������call SetFullScreen��true��
*/
class GDI
{
public:
	GDI() {};
	~GDI() {};
	//��ܵ���
	void Create();
	//��ܵ���
	void Destory();
	void Clean(float color[]);
	void Clean(Color & c);
	void Present(bool isVsync);
	//��ʼ�������������Ǹ����һ�����õĺ���
	//�û�����
	void Initialize(HINSTANCE instance, HWND WndHwnd, HWND TopHwnd, UINT ClientWidth, UINT ClientHeight);
	//��ܵ���
	void SizeChanged(UINT ClientWidth, UINT ClientHeight);

	void ResizeTarget(UINT x, UINT y);

	void SetFullScreen(bool isFullscreen);

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
	//�ָ�RenderTargetView
	void SetRenderTargetView();

	//������pos �ڼ�����ʾģʽ
	void SetFullScreenDisplayMode(int pos);
	//��ȡdisplaymode list
	//����ֵ��mode��Ŀ
	int GetFullScreenDisplayModes(DXGI_MODE_DESC ** c) { *c = mDisplayMode[0].second; return mDisplayMode[0].first; }
	void OpenDefaultBlendState();
	void CloseBlendState();
	
	void SetDefaultSamplerState();
	void CloseSamplerState();
protected:
	ID3D11Device        *mD3dDevice;
	ID3D11DeviceContext *mDeviceContext;
	IDXGISwapChain * mSwapChain;
	ID3D11RenderTargetView * mRenderTargetView;
	
	ID3D11DepthStencilView * mDepthStencilView;
	ID3D11Texture2D * mDepthStencilBuffer;

	ID3D11BlendState * mDisableBlendState;
	ID3D11BlendState * mBlendState;
	ID3D11SamplerState * mLineSamplerState;
	//���� Z ��� ����state
	ID3D11DepthStencilState * mDepthStencilState;
	//��ֹ Z ��� ����state
	ID3D11DepthStencilState * md3dDisableDepthStencilState;
	//��ͨģʽ��Ⱦ��RasterState
	ID3D11RasterizerState * mRasterState;
	//�߿�ģʽ��Ⱦ��RasterState
	ID3D11RasterizerState * mFrameRasterState;
	bool mIsOpenFillSold = true;
	bool mIsOpenZBuffer = true;
	//��ʾ�Ĵ��ھ��
	HWND mHwnd;
	//�������ھ��
	HWND mTopHwnd;

	UINT mWidth;
	UINT mHeight;
	HINSTANCE mInstance;


	std::vector<std::pair<int, DXGI_MODE_DESC *>> mDisplayMode;
	int mNowInDisplayMode;
	std::vector<IDXGIOutput *> mOutputs;
	std::vector<IDXGIAdapter *> mAdapters;
	void SaveDisplayMode(int c, IDXGIOutput * pDXGIOutput);

	//DXGI_ADAPTER_DESC * mAdapters;
private:
#ifdef _DEBUG
	void QueryInterface();
#endif
private:
	DISALLOW_COPY_AND_ASSIGN(GDI);
};
#define PutDebugString(de) de->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(_FUNNAME_XGF_), _FUNNAME_XGF_);