#include "../../Include/Shader.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Log.hpp"
#include <d3dcompiler.h>
Shader::Shader():mGeometryShader(nullptr)
{
}
const EnumLayout SHADER_EL_POSITION3("POSITION", sizeof(XMFLOAT3),0);
const EnumLayout SHADER_EL_POSITION4("POSITION", sizeof(XMFLOAT4), 0);
const EnumLayout SHADER_EL_COLOR("COLOR", sizeof(XMFLOAT4),1);
const EnumLayout SHADER_EL_TEXTURE("TEXCOORD", sizeof(XMFLOAT2),2);
const EnumLayout SHADER_EL_NORMAL("NORMAL", sizeof(XMFLOAT3),3);
const EnumLayout SHADER_EL_SV_POSITION("SV_POSITION", sizeof(XMFLOAT4),4);

const int gCountAtLayout = 5;

Shader::~Shader()
{
}
DXGI_FORMAT GetFormat(int size)
{
	switch (size)
	{
	case 8:
		return DXGI_FORMAT_R32G32_FLOAT;
	case 12:
		return DXGI_FORMAT_R32G32B32_FLOAT;
	case 16:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	return DXGI_FORMAT_R32G32B32A32_FLOAT;
}
void Shader::Initialize(GDI * gdi, const wchar_t * VSname, const wchar_t * PSname, ShaderLayout it[], int len)
{
    if (len <= 0) return;
    if (len > 15) return;
	if (PSname == nullptr) PSname = VSname;
    if (VSname == nullptr) VSname = PSname;
	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pPixelBlob = nullptr;
	ID3DBlob* pVertexBlob = nullptr;
    mGDI = gdi; 
	
	HRESULT hr = D3DCompileFromFile(VSname, NULL,
		NULL, "VS", "vs_4_0", 0, 0, &pVertexBlob, &pErrorBlob);
	if (FAILED(hr))
		if (pErrorBlob == nullptr)
			ReportError(ERROR_SHADER_FILE_NOT_FIND_STRING);
		else
			CheckEx((LPCSTR)pErrorBlob->GetBufferPointer(), hr);
	Check(gdi->GetDevice()->CreateVertexShader(pVertexBlob->GetBufferPointer(),
		pVertexBlob->GetBufferSize(), NULL, &mVertexShader));
	PutDebugString(mVertexShader);
	
	hr = D3DCompileFromFile(PSname, NULL,
		NULL, "PS", "ps_4_0", 0, 0, &pPixelBlob, &pErrorBlob);
	if (FAILED(hr))
		if (pErrorBlob == nullptr)
			ReportError(ERROR_SHADER_FILE_NOT_FIND_STRING);
		else
			CheckEx((LPCSTR)pErrorBlob->GetBufferPointer(), hr);
    Check(gdi->GetDevice()->CreatePixelShader(pPixelBlob->GetBufferPointer(),
		pPixelBlob->GetBufferSize(), NULL, &mPixelShader));
	PutDebugString(mPixelShader);
	
	InitializeOther(it, len,pVertexBlob->GetBufferPointer(),static_cast<unsigned int>(pVertexBlob->GetBufferSize()));
	pPixelBlob->Release();
	pVertexBlob->Release();
}

void Shader::Initialize(GDI * gdi, const wchar_t * VSname, const wchar_t * PSname, const wchar_t * Geometryname, ShaderLayout it[], int len, EnumLayout ot[], int olen)
{
	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pGeometryBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(Geometryname, NULL,
		NULL, "GS", "gs_4_0", 0, 0, &pGeometryBlob, &pErrorBlob);
	if (FAILED(hr))
		if (pErrorBlob == nullptr)
			ReportError(ERROR_SHADER_FILE_NOT_FIND_STRING);
		else
			CheckEx((LPCSTR)pErrorBlob->GetBufferPointer(), hr);

	D3D11_SO_DECLARATION_ENTRY de[16];

	unsigned char countInEachofGroup[gCountAtLayout] = { 0 };
	for (int i = 0; i < olen; i++)
	{
		de[i] = { 0, ot[i].name, countInEachofGroup[ot[i].number],0, (BYTE)(ot[i].size / sizeof(float)), 0 };
		countInEachofGroup[ot[i].number]++;
	}

	gdi->GetDevice()->CreateGeometryShaderWithStreamOutput(pGeometryBlob->GetBufferPointer(),
		pGeometryBlob->GetBufferSize(), de, olen, 0, 0, D3D11_SO_NO_RASTERIZED_STREAM, 0, &mGeometryShader);
	pGeometryBlob->Release();
	Initialize(gdi, VSname, PSname, it, len);
	PutDebugString(mGeometryShader);
}

void Shader::Initialize(GDI * gdi, const unsigned char * constVSShader, unsigned int VSSize, const unsigned char * constPSShader, unsigned int PSSize, ShaderLayout it[], int len)
{
	mGDI = gdi;
	Check(gdi->GetDevice()->CreateVertexShader(constVSShader,
		VSSize, NULL, &mVertexShader));
	PutDebugString(mVertexShader);

	Check(gdi->GetDevice()->CreatePixelShader(constPSShader,
		PSSize, NULL, &mPixelShader));
	PutDebugString(mPixelShader);
	InitializeOther(it, len, constVSShader, VSSize);
}

void Shader::Shutdown()
{
	mCBMatrixBuffer->Release();
	mPixelShader->Release();
	mVertexShader->Release();
	if (mGeometryShader != nullptr)
		mGeometryShader->Release();
    mInputLayout->Release();
	for (int i = 0; i < mCount; i++)
	{
		if (sizePos[i].count > 1)
		{
			delete[] sizePos[i].size.address;
		}
	}
    delete[]sizePos;
}
UINT Shader::GetAllSizeInOneSlot(int i)
{
	if (sizePos[i].count == 1)
	{
		return sizePos[i].size.data;
	}
	else
	{
		UINT u = 0;
		for (int j = 0; j < sizePos[i].count; j++)
			u += sizePos[i].size.address[j];
		return u;
	}
}
void Shader::GetAllSizeInSlot(UINT dout[])
{
	for (int i = 0; i < mCount; i++)
	{
		if (sizePos[i].count == 1)
		{
			dout[i] = sizePos[i].size.data;
		}
		else
		{
			dout[i] = 0;
			for (int j = 0; j < sizePos[i].count; j++)
				dout[i] += sizePos[i].size.address[j];
		}
			
	}
}
void Shader::SetShaderAndInputLayout()
{
	if (mGeometryShader != nullptr)
	{
		ID3D11PixelShader *px = nullptr;
		mGDI->GetDeviceContext()->PSSetShader(px, 0, 0);
		mGDI->GetDeviceContext()->GSSetShader(mGeometryShader, 0, 0);
	}
		
	else
	{
		mGDI->GetDeviceContext()->GSSetShader(nullptr, 0, 0);
		mGDI->GetDeviceContext()->PSSetShader(mPixelShader, 0, 0);
	}
		
    mGDI->GetDeviceContext()->VSSetShader(mVertexShader, 0, 0);
    mGDI->GetDeviceContext()->IASetInputLayout(mInputLayout);
}

void Shader::InitializeOther(ShaderLayout it[], int len, const void * VSpointer, unsigned int VSsize)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;  //动态缓存
	matrixBufferDesc.ByteWidth = sizeof(WVPMatrix);   //结构体大小
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  //CPU访问写
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	Check(mGDI->GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &mCBMatrixBuffer));
	PutDebugString(mCBMatrixBuffer);

	//-------------inputlayout
	D3D11_INPUT_ELEMENT_DESC *layout = new D3D11_INPUT_ELEMENT_DESC[len];
	sizePos = new SlotCan[len];
	UINT data[16];
	bool isconnect = false;
	unsigned int lenBefore = 0;
	unsigned int slot = 0;
	unsigned int  u = 0;// position in a slot
	unsigned char countInEachofGroup[gCountAtLayout] = {0};
	for (int i = 0; i < len; i++)
	{
		layout[i] = { it[i].enumLayout->name, countInEachofGroup[it[i].enumLayout->number], GetFormat(it[i].enumLayout->size),
			slot, lenBefore, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		countInEachofGroup[it[i].enumLayout->number] ++;
		data[u] = it[i].enumLayout->size;
		if(u > 0)
			sizePos[slot].count++;
		else
			sizePos[slot].count = 1;

		if (it[i].mConnet)
		{
			isconnect = true;
			lenBefore += it[i].enumLayout->size;
			u++;
		}
		else
		{
			isconnect = false;
			lenBefore = 0;
			if (sizePos[slot].count == 1)
			{
				sizePos[slot].size.data = data[0];
			}
			else
			{
				sizePos[slot].size.address = new UINT[sizePos[slot].count];
				memcpy_s(sizePos[slot].size.address, sizePos[slot].count * sizeof(UINT), data, sizePos[slot].count * sizeof(UINT));
			}
			u = 0;
			slot++;
		}
			
	}
	mCount = slot;
	Check(mGDI->GetDevice()->CreateInputLayout(layout, len,
		VSpointer, VSsize, &mInputLayout));
	PutDebugString(mInputLayout);
	delete[] layout;
}
void Shader::SetShaderParameter(const WVPMatrix & Matrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto d3dDeviceContext = mGDI->GetDeviceContext();
	d3dDeviceContext->Map(mCBMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy_s(mappedResource.pData, sizeof(WVPMatrix), &Matrix,sizeof(WVPMatrix));
	d3dDeviceContext->Unmap(mCBMatrixBuffer, 0);
	d3dDeviceContext->VSSetConstantBuffers(0, 1, &mCBMatrixBuffer);
	if (mGeometryShader != nullptr)
	{
		d3dDeviceContext->GSSetConstantBuffers(0, 1, &mCBMatrixBuffer);
	}
}
