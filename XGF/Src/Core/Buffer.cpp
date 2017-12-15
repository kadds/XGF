#include "..\..\Include\Buffer.hpp"
#include "..\..\Include\Log.hpp"
namespace XGF
{
	ID3D11Buffer * CreateBuffer(GDI * gdi, int BindFlags, int miscFlags, int byteWidth, int StructureByteStride, D3D11_USAGE bufferUsage, D3D11_CPU_ACCESS_FLAG cpuaccess)
	{
		ID3D11Buffer * buffer;
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = BindFlags;
		desc.ByteWidth = byteWidth;
		desc.MiscFlags = miscFlags;
		desc.StructureByteStride = StructureByteStride;
		desc.Usage = bufferUsage;
		desc.CPUAccessFlags = cpuaccess;
		XGF_Error_Check(gdi->GetDevice()->CreateBuffer(&desc, 0, &buffer), "Failed!");
		return buffer;
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
