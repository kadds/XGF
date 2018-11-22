#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
namespace XGF
{
	class GDI;
	/*
	RTT����Ⱦ������
	��֧��Ƕ��ʹ��
	*/
	class RenderToTexture
	{
	public:
		RenderToTexture();
		~RenderToTexture();
		void Initialize(GDI * gdi, int textureWidth, int textureHeight);
		void Shutdown();
		//��ͼ����Ƶ�Texture
		void SetRenderTarget();
		void Clear(const float color[]);
		void Clear(const SM::Color & c);
		//�ָ�ԭRenderTagetView
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

