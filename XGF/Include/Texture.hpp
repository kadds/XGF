#pragma once
#include <d3d11_1.h>
#include "Defines.hpp"
#include "../Include/DDSTextureLoader.h"
#include "../Include/WICTextureLoader.h"
#include <vector>
namespace XGF
{
	class GDI;
	/*
	纹理用类
	*/
	class Texture
	{
	public:
		Texture();
		Texture(const Texture & t);
		void operator = (const Texture & tx);
		~Texture();

		void LoadFrom(const Texture & texture, float left, float top, float right, float bottom);
		void LoadFromPosition(const Texture & texture, int left, int top, int right, int bottom);

		void LoadDDS(GDI * gdi, const wchar_t * name);
		void Load(ID3D11ShaderResourceView * ShaderResource);
		//加载普通图像文件 如png，jpg，bmp
		void LoadWIC(GDI * gdi, const wchar_t * name);
		ID3D11ShaderResourceView * GetShaderResourceView()const { return mShaderResourceView; }
		void Release() { if (mShaderResourceView && isCreateResourceView) { mShaderResourceView->Release(); mShaderResourceView = nullptr; } }
		float GetTexturePosLeft() const { return left; }
		float GetTexturePosTop() const { return top; }
		float GetTexturePosRight() const { return right; }
		float GetTexturePosBottom() const { return bottom; }
		//void Binding();
	private:
		ID3D11ShaderResourceView * mShaderResourceView;
		//std::vector<std::pair<int,>> mBindingTexturePos;
		float left;
		float top;
		float right;
		float bottom;
		//标示是否清理Texture
		bool isCreateResourceView;
	};


};
