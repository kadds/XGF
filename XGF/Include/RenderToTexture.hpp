#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
namespace XGF
{
	class GDI;
	/*
	RTT，渲染到纹理
	不支持嵌套使用
	*/
	class RenderToTexture
	{
	public:
		RenderToTexture();
		~RenderToTexture();
		void Initialize(GDI * gdi, int textureWidth, int textureHeight);
		void Shutdown();
		//把图像绘制到Texture
		void SetRenderTarget();
		void Clear(const float color[]);
		void Clear(const SM::Color & c);
		//恢复原RenderTagetView
		void SetDefaultRenderTarget();
		ID3D11ShaderResourceView* GetShaderResourceView();
		void ReCreateTexture();
	private:
		ID3D11Texture2D* mRenderTargetTexture;
		ID3D11RenderTargetView* mRenderTargetView;
		ID3D11ShaderResourceView* mShaderResourceView;
		GDI* mGDI;
		int mWidth, mHeight;
	};

};

