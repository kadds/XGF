#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
#include "GDI.hpp"
namespace XGF
{
	enum class BufferUsage 
	{
		CPURead,
		CPUWirte,
	};
	ID3D11Buffer * CreateBuffer(GDI * gdi, int BindFlags, int MiscFlags, int byteWidth,int StructureByteStride, D3D11_USAGE bufferUsage, D3D11_CPU_ACCESS_FLAG cpuaccess);
	ID3D11Buffer * CreateBuffer(GDI * gdi, int BindFlags, int MiscFlags, int byteWidth, int StructureByteStride, D3D11_USAGE bufferUsage, D3D11_CPU_ACCESS_FLAG cpuaccess);

	class IConstantBuffer
	{
	public:
		ID3D11Buffer * GetRawBuffer() { return mRawBuffer; }
		void MapToBuffer(GDI * gdi, void * d, int size);
		void SetName(char * name) { this->name = name; };
		char* GetName() { return name; }
	protected:

		char * name;
		ID3D11Buffer * mRawBuffer;
	};
	ID3D11UnorderedAccessView * CreateUnorderedAccessView(GDI * gdi, ID3D11Buffer * buffer, unsigned int NumElements, DXGI_FORMAT format, D3D11_BUFFER_UAV_FLAG flag);
	ID3D11ShaderResourceView * CreateShaderResourceView(GDI * gdi, ID3D11Buffer * buffer, unsigned int NumElements, DXGI_FORMAT format, D3D11_BUFFEREX_SRV_FLAG flag);
	class ConstantBuffer : public IConstantBuffer
	{
	private:
		void *buffer;
		int size;
	public:
		void Initialize(GDI* gdi, UINT len)
		{
			mRawBuffer = CreateBuffer(gdi, D3D11_BIND_CONSTANT_BUFFER ,0, len,0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			size = len;
			buffer = new char[len];
		};
		void Shutdown() { mRawBuffer->Release(); delete[] buffer; };
		int GetSize() { return size; }
		void * GetBufferPoint() { return buffer; };
		void MapToGPU(GDI * gdi);
	};
}

