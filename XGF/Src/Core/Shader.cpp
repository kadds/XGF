#include "../../Include/Shader.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Log.hpp"
#include "../../Include/Buffer.hpp"
#include <d3dcompiler.h>
namespace XGF {
	Shader::Shader()
	{
	}
	const EnumLayout SHADER_EL_POSITION3("POSITION", sizeof(XMFLOAT3), 0);
	const EnumLayout SHADER_EL_POSITION4("POSITION", sizeof(XMFLOAT4), 0);
	const EnumLayout SHADER_EL_COLOR("COLOR", sizeof(XMFLOAT4), 1);
	const EnumLayout SHADER_EL_TEXTURE("TEXCOORD", sizeof(XMFLOAT2), 2);
	const EnumLayout SHADER_EL_NORMAL("NORMAL", sizeof(XMFLOAT3), 3);
	const EnumLayout SHADER_EL_SV_POSITION("SV_POSITION", sizeof(XMFLOAT4), 4);

	const int gCountAtLayout = 5;

	Shader::~Shader()
	{
	}
	DXGI_FORMAT GetFormat(int size)
	{
		switch (size)
		{
		case 4:
			return DXGI_FORMAT_R32_FLOAT;
		case 8:
			return DXGI_FORMAT_R32G32_FLOAT;
		case 12:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case 16:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		XGF_ReportWarn("Format is unsupported", "return DXGI_FORMAT_R32G32B32A32_FLOAT");
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}

	void ComputeShader::Initialize(GDI* gdi, const wchar_t* CSname, int maxcount, int constantByteWidth, int inputByteWidth, int outputByteWidth)
	{
		mGDI = gdi;
		ID3DBlob* pErrorBlob = nullptr;
		ID3DBlob* pComputerBlob = nullptr;

		HRESULT hr = D3DCompileFromFile(CSname, NULL,
			NULL, "CS", gdi->CheckFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ? "cs_5_0" : "cs_4_0", 0, 0, &pComputerBlob, &pErrorBlob);
		if (FAILED(hr))
			if (pErrorBlob == nullptr)
				XGF_ReportError("ShaderFile no find", "");
			else
				XGF_Error_Check(hr, (LPCSTR)pErrorBlob->GetBufferPointer());
		XGF_Error_Check(gdi->GetDevice()->CreateComputeShader(pComputerBlob->GetBufferPointer(),
			pComputerBlob->GetBufferSize(), nullptr, &mComputeShader), "");
		pComputerBlob->Release();
		PutDebugString(mComputeShader);

		mOutputBuffer[0] =CreateBuffer(gdi, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, maxcount * inputByteWidth, inputByteWidth, D3D11_USAGE_DEFAULT, D3D11_CPU_ACCESS_READ);
		mOutputBuffer[1] = CreateBuffer(gdi, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, maxcount * inputByteWidth, inputByteWidth, D3D11_USAGE_DEFAULT, D3D11_CPU_ACCESS_READ);


		D3D11_SHADER_RESOURCE_VIEW_DESC DescRV;
		ZeroMemory(&DescRV, sizeof(DescRV));
		DescRV.Format = DXGI_FORMAT_UNKNOWN;
		DescRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		DescRV.Buffer.FirstElement = 0;
		//DescRV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;

		XGF_Error_Check(gdi->GetDevice()->CreateShaderResourceView(mOutputBuffer[0], &DescRV, &mInputShaderResourceView[0]), "");
		XGF_Error_Check(gdi->GetDevice()->CreateShaderResourceView(mOutputBuffer[1], &DescRV, &mInputShaderResourceView[1]), "");

		D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
		ZeroMemory(&DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		DescUAV.Format = DXGI_FORMAT_UNKNOWN;
		DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		DescUAV.Buffer.FirstElement = 0;
		//DescUAV.Buffer.NumElements = desc.ByteWidth / desc.StructureByteStride;

		XGF_Error_Check(gdi->GetDevice()->CreateUnorderedAccessView(mOutputBuffer[0], &DescUAV, &mUnorderedAccessView[0]), "");
		XGF_Error_Check(gdi->GetDevice()->CreateUnorderedAccessView(mOutputBuffer[1], &DescUAV, &mUnorderedAccessView[1]), "");
	}

	void ComputeShader::Shutdown()
	{
		mComputeShader->Release();
		mUnorderedAccessView[0]->Release();
		mUnorderedAccessView[1]->Release();
		mOutputBuffer[0]->Release();
		mOutputBuffer[1]->Release();
		mInputShaderResourceView[1]->Release();
		mInputShaderResourceView[0]->Release();
	}

	void ComputeShader::Run()
	{
		mGDI->GetDeviceContext()->CSSetShader(mComputeShader, nullptr, 0);
		//mGDI->GetDeviceContext()->CSSetConstantBuffers(0, 1, &mCBMatrixBuffer);
		ID3D11ShaderResourceView* aRViews[1] = { mInputShaderResourceView[0] };
		mGDI->GetDeviceContext()->CSSetShaderResources(0, 1, aRViews);
		ID3D11UnorderedAccessView* aUAViews[1] = { mUnorderedAccessView[1] };
		mGDI->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, aUAViews, (UINT*)(&aUAViews));

		mGDI->GetDeviceContext()->Dispatch(32, 1, 1);

		D3D11_QUERY_DESC pQueryDesc;
		pQueryDesc.Query = D3D11_QUERY_EVENT;
		pQueryDesc.MiscFlags = 0;
		ID3D11Query *pEventQuery;
		mGDI->GetDevice()->CreateQuery(&pQueryDesc, &pEventQuery);
		mGDI->GetDeviceContext()->End(pEventQuery); // 在 pushbuffer 中插入一个篱笆
		while (mGDI->GetDeviceContext()->GetData(pEventQuery, NULL, 0, 0) == S_FALSE) {} // 自旋等待事件结束
		pEventQuery->Release();
		ID3D11ComputeShader * sh = 0;
		mGDI->GetDeviceContext()->CSGetShader(&sh, nullptr, nullptr);
		//ID3D11Buffer *nullBf = 0;
		ID3D11ShaderResourceView *nullres = 0;
		//mGDI->GetDeviceContext()->CSSetConstantBuffers(0, 1, &nullBf);

		ID3D11UnorderedAccessView * nulluav = 0;
		mGDI->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &nulluav, 0);
		mGDI->GetDeviceContext()->CSSetShaderResources(0, 1, &nullres);
		struct MyStruct
		{
			Point pos;
			Point pos2;
		};
		std::swap(mOutputBuffer[0], mOutputBuffer[1]);
		std::swap(mUnorderedAccessView[0], mUnorderedAccessView[1]);
		std::swap(mInputShaderResourceView[0], mInputShaderResourceView[1]);
	}
	void PixelShader::Initialize(GDI * gdi, const wchar_t * PSname)
	{
		ID3DBlob* pErrorBlob = nullptr;
		ID3DBlob* pPixelBlob = nullptr;
		mGDI = gdi;

		HRESULT hr = D3DCompileFromFile(PSname, NULL,
			NULL, "PS", "ps_4_0", 0, 0, &pPixelBlob, &pErrorBlob);
		if (FAILED(hr))
			if (pErrorBlob == nullptr)
				XGF_ReportError("Pixel Shader File Not Find!", PSname);
			else
				XGF_Error_Check(hr, (LPCSTR)pErrorBlob->GetBufferPointer());
		XGF_Error_Check(gdi->GetDevice()->CreatePixelShader(pPixelBlob->GetBufferPointer(),
			pPixelBlob->GetBufferSize(), NULL, &mPixelShader), "CreatePixelShader Failed");
		PutDebugString(mPixelShader);
		pPixelBlob->Release();
	}

	void PixelShader::Initialize(GDI * gdi, const unsigned char * PScode, unsigned int codeLen)
	{
		XGF_Error_Check(gdi->GetDevice()->CreatePixelShader(PScode,
			codeLen, NULL, &mPixelShader), "CreatePixelShader Failed");
		PutDebugString(mPixelShader);
		mGDI = gdi;
	}

	void PixelShader::Shutdown()
	{
		mPixelShader->Release();
	}

	void PixelShader::SetShaderConstantParameter(IConstantBuffer * cb)
	{
		auto t = cb->GetRawBuffer();
		mGDI->GetDeviceContext()->PSSetConstantBuffers(0, 1, &t);
	}

	void VertexShader::Initialize(GDI * gdi, const wchar_t * VSname, ShaderLayout layout[], int layoutCount)
	{
		ID3DBlob* pErrorBlob = nullptr;
		ID3DBlob* pVertexBlob = nullptr;
		mGDI = gdi;

		HRESULT hr = D3DCompileFromFile(VSname, NULL,
			NULL, "VS", "vs_4_0", 0, 0, &pVertexBlob, &pErrorBlob);
		if (FAILED(hr))
			if (pErrorBlob == nullptr)
				XGF_ReportError("Vertex Shader File Not Find!", VSname);
			else
				XGF_Error_Check(hr, (LPCSTR)pErrorBlob->GetBufferPointer());
		Initialize(gdi, (unsigned char *)pVertexBlob->GetBufferPointer(), (int)pVertexBlob->GetBufferSize(), layout, layoutCount);
		
		pVertexBlob->Release();
	}

	void VertexShader::Initialize(GDI * gdi, const unsigned char * VScode, unsigned int codeLen, ShaderLayout layout[], int layoutCount)
	{
		XGF_Error_Check(gdi->GetDevice()->CreateVertexShader(VScode,
			codeLen, NULL, &mVertexShader), "CreateVertexShader Failed");
		PutDebugString(mVertexShader);
		mGDI = gdi;

		//-------------inputlayout
		D3D11_INPUT_ELEMENT_DESC *d3dlayout = new D3D11_INPUT_ELEMENT_DESC[layoutCount];
		mSlotStride = new SlotCan[layoutCount];
		unsigned int lenBefore = 0;
		int itemp = 0;
		unsigned int slot = 0;
		unsigned char countInEachofGroup[gCountAtLayout] = { 0 };
		for (int i = 0; i < layoutCount; i++)
		{
			mSlotStride[i].size = layout[i].enumLayout->size;
			mSlotStride[i].nextIsASlot = layout[i].mConnet;
			d3dlayout[i] = { layout[i].enumLayout->name, countInEachofGroup[layout[i].enumLayout->number], GetFormat(layout[i].enumLayout->size), slot, lenBefore, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			if (layout[i].mConnet)
			{
				lenBefore += layout[i].enumLayout->size;
			}
			else
			{
				slot++;
				lenBefore = 0;
			}
			countInEachofGroup[layout[i].enumLayout->number]++;
		}
		mCount = layoutCount;
		XGF_Error_Check(gdi->GetDevice()->CreateInputLayout(d3dlayout, layoutCount,
			VScode, codeLen, &mInputLayout), "CreateInputLayout Error");
		delete[] d3dlayout;
	}

	void VertexShader::Shutdown()
	{
		delete[] mSlotStride;
		mVertexShader->Release();
		mInputLayout->Release();
	}

	void VertexShader::SetInputLayout()
	{
		mGDI->GetDeviceContext()->IASetInputLayout(mInputLayout);
	}

	int VertexShader::GetSlotCount()
	{
		int rt = 0;
		for (int i = 0; i < mCount; i++)
		{
			if (!mSlotStride[i].nextIsASlot)
				rt++;
		}
		return rt;
	}

	void VertexShader::SetShaderConstantParameter(IConstantBuffer * cb)
	{
		auto t = cb->GetRawBuffer();
		mGDI->GetDeviceContext()->VSSetConstantBuffers(0, 1, &t);
	}

	void VertexShader::GetStride(unsigned int stride[])
	{
		int rt = mSlotStride[0].size;
		int pos = 0;
		for (int i = 0; i < mCount; i++)
		{
			if (!mSlotStride[i].nextIsASlot)
			{
				stride[pos] = rt;
				pos++;
				rt = mSlotStride[i + 1].size;
			}
			else
			{
				rt += mSlotStride[i].size;
			}
				
		}
	}

	int VertexShader::GetSize(int position)
	{
		return mSlotStride[position].size;
	}

	void GeometryShader::Initialize(GDI * gdi, const wchar_t * GSname)
	{
		ID3DBlob* pErrorBlob = nullptr;
		ID3DBlob* pGeometryBlob = nullptr;
		mGDI = gdi;

		HRESULT hr = D3DCompileFromFile(GSname, NULL,
			NULL, "GS", "gs_4_0", 0, 0, &pGeometryBlob, &pErrorBlob);
		if (FAILED(hr))
			if (pErrorBlob == nullptr)
				XGF_ReportError("Geometry Shader File Not Find!", GSname);
			else
				XGF_Error_Check(hr, (LPCSTR)pErrorBlob->GetBufferPointer());
		XGF_Error_Check(gdi->GetDevice()->CreateGeometryShader(pGeometryBlob->GetBufferPointer(),
			pGeometryBlob->GetBufferSize(), NULL, &mGeometryShader), "CreateGeometryShader Failed");
		PutDebugString(mGeometryShader);
	}

	void GeometryShader::Shutdown()
	{
		mGeometryShader->Release();
	}

	void Shaders::BindShader()
	{
		auto dev = vs->mGDI->GetDeviceContext();
		dev->VSSetShader(vs->mVertexShader, 0, 0);
		if(ps != nullptr)
			dev->PSSetShader(ps->mPixelShader, 0, 0);
		if(gs != nullptr)
			dev->GSSetShader(gs->mGeometryShader, 0, 0);
	}

	void Shaders::UnBindShader()
	{
		void* null = 0 ;
		auto dev = vs->mGDI->GetDeviceContext();
		dev->VSSetShader((ID3D11VertexShader *)null, 0, 0);
		if (ps != nullptr)
			dev->PSSetShader((ID3D11PixelShader *)null, 0, 0);
		if (gs != nullptr)
			dev->GSSetShader((ID3D11GeometryShader *)null, 0, 0);
	}

}
