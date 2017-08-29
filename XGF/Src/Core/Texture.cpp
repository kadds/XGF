#include "../../Include/Texture.hpp"
#include "../../Include/Log.hpp"
#include "../../Include/GDI.hpp"
Texture::Texture():isCreateResourceView(false)
{
}
Texture::Texture(const Texture & t) : isCreateResourceView(false)
{
    mShaderResourceView = t.mShaderResourceView;
    left = t.left;
    top = t.top;
    right = t.right;
    bottom = t.bottom;
}
void Texture::operator=(const Texture & t)
{
	mShaderResourceView = t.mShaderResourceView;
	left = t.left;
	top = t.top;
	right = t.right;
	bottom = t.bottom;
	isCreateResourceView = false;
}
Texture::~Texture()
{
	//Release();
}
void Texture::LoadDDS(GDI * gdi, const wchar_t * name)
{
	//ID3D11Resource * texture;
	Check(DirectX::CreateDDSTextureFromFile(gdi->GetDevice(), gdi->GetDeviceContext(), name,nullptr,&mShaderResourceView));
	PutDebugString(mShaderResourceView);
    left = top = 0.0f;
    right = bottom = 1.0f;
    isCreateResourceView = true;
}

void Texture::Load(ID3D11ShaderResourceView * ShaderResource)
{
	mShaderResourceView = ShaderResource;
}
void Texture::LoadFrom(const Texture & texture, float left, float top, float right, float bottom)
{
    mShaderResourceView = texture.mShaderResourceView;
    this->left = left;
    this->top = top;
    this->right = right;
    this->bottom = bottom;
}
void Texture::LoadFromPosition(const Texture & texture, int left, int top, int right, int bottom)
{
    mShaderResourceView = texture.mShaderResourceView;
    this->left = static_cast<float>(left);
    this->top = static_cast<float>(top);
    this->right = static_cast<float>(right);
    this->bottom = static_cast<float>(bottom);
}
void Texture::LoadWIC(GDI * gdi, const wchar_t * name)
{
	Check(DirectX::CreateWICTextureFromFile(gdi->GetDevice(), gdi->GetDeviceContext(), name, nullptr, &mShaderResourceView));
	PutDebugString(mShaderResourceView);
    left = top = 0.0f;
    right = bottom = 1.0f;
    isCreateResourceView = true;
}
