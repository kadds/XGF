#include "../../Include/Shader.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Log.hpp"
#include <d3dcompiler.h>
Shader::Shader():mGeometryShader(nullptr)
{
}


Shader::~Shader()
{
}

void Shader::Initialize(GDI * gdi, const wchar_t * VSname, const wchar_t * PSname, InputType it[], int len)
{
    if (len <= 0) return;
    if (len > 15) return;
	if (PSname == nullptr) PSname = VSname;
    if (VSname == nullptr) VSname = PSname;
	ID3DBlob* pErrorBlob = nullptr;
	ID3DBlob* pPixelBlob = nullptr;
	ID3DBlob* pVertexBlob = nullptr;
    mGDI = gdi; 
	mCount = len;
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
	
	//devc->IASetInputLayout(inputLayout);
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

void Shader::Initialize(GDI * gdi, const wchar_t * VSname, const wchar_t * PSname, const wchar_t * Geometryname, InputType it[], int len)
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
	Check(gdi->GetDevice()->CreateGeometryShader(pGeometryBlob->GetBufferPointer(),
		pGeometryBlob->GetBufferSize(), NULL, &mGeometryShader));
	PutDebugString(mGeometryShader);

	Initialize(gdi, VSname, PSname, it, len);

}

void Shader::Initialize(GDI * gdi, const unsigned char * constVSShader, unsigned int VSSize, const unsigned char * constPSShader, unsigned int PSSize, InputType it[], int len)
{
	mGDI = gdi;
	mCount = len;
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
    mInputLayout->Release();
    delete[]sizePos;
}
void Shader::SetShaderAndInputLayout()
{
	if (mGeometryShader != nullptr)
		mGDI->GetDeviceContext()->GSSetShader(mGeometryShader, 0, 0);
	else
		mGDI->GetDeviceContext()->GSSetShader(nullptr, 0, 0);
    mGDI->GetDeviceContext()->PSSetShader(mPixelShader, 0, 0);
    mGDI->GetDeviceContext()->VSSetShader(mVertexShader, 0, 0);
    mGDI->GetDeviceContext()->IASetInputLayout(mInputLayout);
}
void Shader::InitializeOther(InputType it[], int len, const void * VSpointer, unsigned int VSsize)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;  //动态缓存
	matrixBufferDesc.ByteWidth = sizeof(WVPMatrix);   //结构体大小,必须为16字节倍数
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;  //CPU访问写
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	Check(mGDI->GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &mCBMatrixBuffer));
	PutDebugString(mCBMatrixBuffer);
	//mInputLayoutLen = len;
	//-------------inputlayout
	D3D11_INPUT_ELEMENT_DESC *layout = new D3D11_INPUT_ELEMENT_DESC[len];
	sizePos = new unsigned[len];
	unsigned count = len;
	unsigned lg = 0;// D3D11_APPEND_ALIGNED_ELEMENT;
	for (unsigned i = 0; i < count; i++)
	{
		if (it[i] == SHADER_INPUTLAYOUT_POSITION)
		{
			layout[i] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
				i, lg, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			sizePos[i] = sizeof(DirectX::XMFLOAT3);
		}
		else if (it[i] == SHADER_INPUTLAYOUT_COLOR)
		{
			layout[i] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
				i, lg, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			sizePos[i] = sizeof(DirectX::XMFLOAT4);
		}
		else if (it[i] == SHADER_INPUTLAYOUT_TEXTURE)
		{
			layout[i] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
				i, lg, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			sizePos[i] = sizeof(DirectX::XMFLOAT2);
		}

	}

	Check(mGDI->GetDevice()->CreateInputLayout(layout, len,
		VSpointer, VSsize, &mInputLayout));
	PutDebugString(mInputLayout);
	
	delete[] layout;
}
void Shader::SetShaderParameter(const WVPMatrix & Matrix)
{

	//_declspec (align(16)) WVPMatrix Matrix2;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
    auto d3dDeviceContext = mGDI->GetDeviceContext();
	//锁定常量缓存,这时候常量缓存和子资源关联在一起,动态常量缓存才能使用
	d3dDeviceContext->Map(mCBMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//获取指向常量缓存数据的指针
	memcpy_s(mappedResource.pData, sizeof(WVPMatrix), &Matrix,sizeof(WVPMatrix));
	//解锁常量缓存
	d3dDeviceContext->Unmap(mCBMatrixBuffer, 0);
	//设置在顶点缓存中常量缓存的位置,注册号
	//第一,设置在VertexShader的常量缓存的值(带着更新的值)
	d3dDeviceContext->VSSetConstantBuffers(0, 1, &mCBMatrixBuffer);
	//第二,设置在PixelShader的纹理资源
}
