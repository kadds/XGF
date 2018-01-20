#pragma once
#define NOMINMAX
#include <d3d11_1.h>
#include "Defines.hpp"
#include "../Include/DDSTextureLoader.h"
#include "../Include/WICTextureLoader.h"
#include <vector>
namespace XGF
{

	class GDI;
	class TextureResource
	{
	private:
		ID3D11ShaderResourceView * mShaderResourceView;
		ID3D11Texture2D * mTexture;
		struct Size
		{
			UINT Width;
			UINT Height;
		};
	public:
		Size mSize;
		TextureResource();
		bool Load(GDI * gdi, void * mem, size_t size);
		bool Load(GDI * gdi, std::wstring fullPath);
		void Release();
		ID3D11ShaderResourceView * GetShaderResourceView()const { return mShaderResourceView; }
		ID3D11Texture2D * GetTexture() const { return mTexture; }
	};

	/*
	ÎÆÀíÓÃÀà
	*/
	class Texture
	{
	public:
		struct Rect
		{
			int Left;
			int Right;
			int Top;
			int Bottom;
		};

		Texture();
		~Texture();
		Texture(TextureResource & tres);
		Texture(TextureResource & tres, Rect & rc);
	public:
		void SetTextureResource(TextureResource * tres) { mTextureResource = tres; };
		// In Point4, x is left, y is top, z is right, w is bottom
		void SetRectangleNormalization(Point4 & p4) { mTextureRectangleNormalization = p4; };
		void Set9PathInnerRect(Rect & rc);
		void Set9PathBorderSize(int border);

		void SetRectangle(Rect & rc);

		float GetLeft() const  { return mTextureRectangleNormalization.x; };
		float GetTop() const  { return mTextureRectangleNormalization.y; };
		float GetRight() const  { return mTextureRectangleNormalization.z; };
		float GetBottom() const  { return mTextureRectangleNormalization.w; };

		bool Is9Path() const{ return mIs9Path; };

		float Get9PathLeft() const { return m9Path.x; };
		float Get9PathTop() const { return m9Path.y; };
		float Get9PathRight() const { return m9Path.z; };
		float Get9PathBottom() const { return m9Path.w; };

		ID3D11ShaderResourceView * GetRawTexture() const { return mTextureResource->GetShaderResourceView(); };
	private:
		TextureResource * mTextureResource;
		// x : left, y:top, z: right, w: bottom
		Point4 mTextureRectangleNormalization;

		bool mIs9Path;
		Point4 m9Path;
	};


};
