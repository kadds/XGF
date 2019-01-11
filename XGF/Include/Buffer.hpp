#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
#include "GDI.hpp"
#include "Context.hpp"

namespace XGF
{
	enum class BufferUsage 
	{
		CPURead,
		CPUWirte,
	};
	ID3D11Buffer * CreateBuffer(GDI & gdi, int BindFlags, int miscFlags, int byteWidth,int StructureByteStride, D3D11_USAGE bufferUsage, D3D11_CPU_ACCESS_FLAG cpuaccess);
	ID3D11Buffer * CreateBuffer(GDI & gdi, int BindFlags, int MiscFlags, int byteWidth, int StructureByteStride, D3D11_USAGE bufferUsage, D3D11_CPU_ACCESS_FLAG cpuaccess);

	class IConstantBuffer
	{
	public:
		ID3D11Buffer* GetRawBuffer();
		void MapToBuffer(GDI & gdi, void * d, int size);
		void SetName(char* name);;
		char* GetName();
	protected:

		char * name;
		ID3D11Buffer * mRawBuffer;
	};
	ID3D11UnorderedAccessView * CreateUnorderedAccessView(GDI & gdi, ID3D11Buffer * buffer, unsigned int NumElements, DXGI_FORMAT format, D3D11_BUFFER_UAV_FLAG flag);
	ID3D11ShaderResourceView * CreateShaderResourceView(GDI & gdi, ID3D11Buffer * buffer, unsigned int NumElements, DXGI_FORMAT format, D3D11_BUFFEREX_SRV_FLAG flag);
	class ConstantBuffer : public IConstantBuffer
	{
	private:
		void *buffer;
		int size;
	public:
		ConstantBuffer();;
		void Initialize(UINT len);;
		void Shutdown();;
		int GetSize();
		void* GetBufferPoint();;
		void MapToGPU(GDI & gdi);
	};
}

