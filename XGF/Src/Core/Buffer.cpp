#include "..\..\Include\Buffer.hpp"
#include "..\..\Include\Logger.hpp"
namespace XGF
{
	ID3D11Buffer * CreateBuffer(GDI * gdi, int BindFlags, int miscFlags, int byteWidth, int StructureByteStride, D3D11_USAGE bufferUsage, D3D11_CPU_ACCESS_FLAG cpuaccess)
	{
		XGF_ASSERT(byteWidth > 0);
		XGF_ASSERT(byteWidth % 4 == 0);
		ID3D11Buffer * buffer;
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = BindFlags;
		desc.ByteWidth = byteWidth;
		desc.MiscFlags = miscFlags;
		desc.StructureByteStride = StructureByteStride;
		desc.Usage = bufferUsage;
		desc.CPUAccessFlags = cpuaccess;
		XGF_Error_Check(Application, gdi->GetDevice()->CreateBuffer(&desc, 0, &buffer), "Create buffer failed");
		PutDebugString(buffer);
		return buffer;
	}

	ID3D11UnorderedAccessView * CreateUnorderedAccessView(GDI * gdi, ID3D11Buffer * buffer, unsigned int NumElements, DXGI_FORMAT format, D3D11_BUFFER_UAV_FLAG flag)
	{
		ID3D11UnorderedAccessView * uav;
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		desc.Format = format;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = flag;
		desc.Buffer.NumElements = NumElements;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		XGF_Error_Check(Application, gdi->GetDevice()->CreateUnorderedAccessView(buffer, &desc, &uav), "Create UnorderedAccessView failed");
		PutDebugString(uav);
		return uav;
	}

	ID3D11ShaderResourceView * CreateShaderResourceView(GDI * gdi, ID3D11Buffer * buffer, unsigned int NumElements, DXGI_FORMAT format, D3D11_BUFFEREX_SRV_FLAG flag)
	{
		ID3D11ShaderResourceView * srv;
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		desc.Format = format;
		desc.BufferEx.FirstElement = 0;
		desc.BufferEx.Flags = flag;
		desc.BufferEx.NumElements = NumElements;
		XGF_Error_Check(Application, gdi->GetDevice()->CreateShaderResourceView(buffer, &desc, &srv), "Create ShaderResourceView failed");
		PutDebugString(srv);
		return srv;
	}

	void IConstantBuffer::MapToBuffer(GDI * gdi, void * d, int size)
	{
		D3D11_MAPPED_SUBRESOURCE sub;
		gdi->GetDeviceContext()->Map(mRawBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
		memcpy(sub.pData, d, size);
		gdi->GetDeviceContext()->Unmap(mRawBuffer, 0);
	}

	void ConstantBuffer::MapToGPU(GDI * gdi)
	{
		MapToBuffer(gdi, buffer, size);
	}

}
