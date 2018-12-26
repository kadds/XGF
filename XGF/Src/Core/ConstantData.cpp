#include "..\..\Include\ConstantData.hpp"
#include "..\..\Include\ShaderConst.hpp"
#include "..\..\Include\Logger.hpp"
#include <random>
namespace XGF
{
	ConstantData ConstantData::mConstantData;
	UINT ConstantData::mRanowmSize = 128;
	const std::string ConstantData::mFileNames[] = {
		"BasicMaterial.P.hlsl",
		"BasicMaterial.PT.hlsl", 
		"BasicMaterial.PC.hlsl",
		"BasicMaterial.PN.hlsl",
		"BasicMaterial.PTC.hlsl",
		"BasicMaterial.PTC.Alpha.hlsl",
		"BasicMaterial.PNT.hlsl",
		"BasicMaterial.PNC.hlsl", 
		"BasicMaterial.PNTC.hlsl"};
	void ConstantData::Initialize(GDI * gdi)
	{
		for(int i = 0; i < mMaxConstantShaderCount; i++)
		{
			auto fileData = Tools::LoadFromFile(mFileNames[i]);
			auto vs = new VertexShader();
			auto ps = new PixelShader();

			vs->Initialize(gdi, (unsigned char*)fileData.first.get(), fileData.second, mFileNames[i].c_str());
			ps->Initialize(gdi, (unsigned char*)fileData.first.get(), fileData.second, mFileNames[i].c_str());

			mShaders[i] = { vs, ps, nullptr };
		}
		
		//random
		ID3D11Texture1D *texture;
		D3D11_TEXTURE1D_DESC desc;
		desc.Width = mRanowmSize;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.ArraySize = 1;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.MiscFlags = 0;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		std::random_device rdDevice;
		std::mt19937 mrandom(rdDevice());
		std::uniform_real_distribution<> rtt(0);
		SM::Vector4 * buffer = new SM::Vector4[mRanowmSize];
		D3D11_SUBRESOURCE_DATA __subData;
		__subData.pSysMem = buffer;
		__subData.SysMemPitch = 0;
		__subData.SysMemSlicePitch = mRanowmSize;
		for (unsigned int i = 0; i < mRanowmSize; i++)
			buffer[i] = { (float)rtt(mrandom) ,(float)rtt(mrandom) ,(float)rtt(mrandom) ,(float)rtt(mrandom) };
		gdi->GetDevice()->CreateTexture1D(&desc, &__subData, &texture);
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = desc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		viewDesc.Texture1D.MipLevels = desc.MipLevels;
		viewDesc.Texture1D.MostDetailedMip = 0;
		XGF_Error_Check(Application, gdi->GetDevice()->CreateShaderResourceView(texture, &viewDesc, &mRandomSRV), "Create Ranom SRV Failed!");//Create Random SRV
		PutDebugString(mRandomSRV);
		texture->Release();
		delete[] buffer;
	}

	void ConstantData::Shutdown()
	{
		mRandomSRV->Release();
		for (int i = 0; i < mMaxConstantShaderCount; i++)
		{
			if (mShaders[i].vs) {
				mShaders[i].vs->Shutdown();
				delete mShaders[i].vs;
				mShaders[i].vs = nullptr;
			}
			if (mShaders[i].ps) {
				mShaders[i].ps->Shutdown();
				delete mShaders[i].ps;
				mShaders[i].ps = nullptr;
			}
			if (mShaders[i].gs) {
				mShaders[i].gs->Shutdown();
				delete mShaders[i].gs;
				mShaders[i].gs = nullptr;
			}
		}
	}
}
