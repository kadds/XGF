#include "../../Include/Shader.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Log.hpp"
#include "../../Include/Buffer.hpp"

namespace XGF {
	Shader::Shader()
	{
	}

	Shader::~Shader()
	{
		if (mCBufferInfo != nullptr)
			delete[] mCBufferInfo;
		if (mTexture2D != nullptr)
			delete[] mTexture2D;
		if (mSamplerState != nullptr)
			delete[] mSamplerState;
	}
	void Shader::ReflectStage(ID3D11ShaderReflection * reflector)
	{
		mTexture2D = 0;
		mSamplerState = 0;
		mBindTextureCount = 0;
		mBindSamplerCount = 0;
		D3D11_SHADER_DESC shaderDesc;
		reflector->GetDesc(&shaderDesc);
		
		//constant Buffer
		int constantBufferCount = shaderDesc.ConstantBuffers;
		mCBufferCount = constantBufferCount;
		if (mCBufferCount > 0)
		{
			mCBufferInfo = new CBufferInfo[shaderDesc.ConstantBuffers];
			ID3D11ShaderReflectionConstantBuffer * cbuffer;
			D3D11_SHADER_BUFFER_DESC shaderBuferDesc;
			for (int i = 0; i < constantBufferCount; i++)
			{
				cbuffer = reflector->GetConstantBufferByIndex(i);
				cbuffer->GetDesc(&shaderBuferDesc);
				
				if (shaderBuferDesc.Type == _D3D_CBUFFER_TYPE::D3D11_CT_CBUFFER || shaderBuferDesc.Type == _D3D_CBUFFER_TYPE::D3D11_CT_TBUFFER)
				{
					
					mCBufferInfo[i].name = shaderBuferDesc.Name;
					mCBufferInfo[i].size = shaderBuferDesc.Size;
				}
			}
		}
		else
			mCBufferInfo = nullptr;
		TextureInfo tinfo[16];
		SamplerInfo sinfo[16];
		//parse all bind info
		D3D11_SHADER_INPUT_BIND_DESC bdec;
		for (int i = 0; i < shaderDesc.BoundResources; i++)
		{
			reflector->GetResourceBindingDesc(i, &bdec);
			if (bdec.Type == _D3D_CBUFFER_TYPE::D3D11_CT_CBUFFER || bdec.Type == _D3D_CBUFFER_TYPE::D3D11_CT_TBUFFER)
			{
				for (int i = 0; i < constantBufferCount; i++)
				{
					if (strcmp(mCBufferInfo[i].name, bdec.Name) == 0)
					{
						mCBufferInfo[i].slot = bdec.BindPoint;
						break;
					}
				}

			}
			else if (bdec.Type == D3D_SIT_TEXTURE)
			{
				tinfo[mBindTextureCount].name = bdec.Name;
				tinfo[mBindTextureCount].slot = bdec.BindPoint;
				mBindTextureCount++;
			}
			else if (bdec.Type == D3D_SIT_SAMPLER)
			{
				sinfo[mBindSamplerCount].name = bdec.Name;
				sinfo[mBindSamplerCount].slot = bdec.BindPoint;
				mBindSamplerCount++;
			}
		
		}

		// end

		mTexture2D = new TextureInfo[mBindTextureCount];
		mSamplerState = new SamplerInfo[mBindSamplerCount];
		memcpy(mTexture2D, tinfo, sizeof(TextureInfo) * mBindTextureCount);
		memcpy(mSamplerState, tinfo, sizeof(SamplerInfo) * mBindSamplerCount);
	}
	unsigned int Shader::GetCBufferSize(int index)
	{
		return mCBufferInfo[index].size;
	}
	unsigned int Shader::GetCBufferCount()
	{
		return mCBufferCount;
	}
	GDI * Shader::GetGDI()
	{
		return mGDI;
	}

	unsigned int Shader::GetCBufferSlot(int index)
	{
		return mCBufferInfo[index].slot;
	}

	int Shader::GetSamplerStateCount()
	{
		return mBindSamplerCount;
	}

	unsigned int Shader::GetSamplerStateSlot(int index)
	{
		return mSamplerState[index].slot;
	}

	int Shader::GetTextureCount()
	{
		return mBindTextureCount;
	}

	unsigned int Shader::GetTextureSlot(int index)
	{
		return mTexture2D[index].slot;
	}


	void ComputeShader::Initialize(GDI * gdi, const wchar_t * CSname)
	{
		
		ID3DBlob* pErrorBlob = nullptr;
		ID3DBlob* pComputerBlob = nullptr;

		HRESULT hr = D3DCompileFromFile(CSname, NULL,
			NULL, "CS", gdi->CheckFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ? "cs_5_0" : "cs_4_0", 0, 0, &pComputerBlob, &pErrorBlob);
		if (FAILED(hr))
			if (pErrorBlob == nullptr)
				XGF_ReportError("ShaderFile no find", "");
			else
				XGF_Error_Check(hr, (LPCSTR)pErrorBlob->GetBufferPointer());
		Initialize(gdi, (unsigned char *)pComputerBlob->GetBufferPointer(), pComputerBlob->GetBufferSize());
		pComputerBlob->Release();
		PutDebugString(mComputeShader);
	}

	void ComputeShader::Initialize(GDI * gdi, const unsigned char * CScode, unsigned int codeLen)
	{
		mGDI = gdi;
		XGF_Error_Check(gdi->GetDevice()->CreateComputeShader(CScode,
			codeLen, nullptr, &mComputeShader), "");
		ID3D11ShaderReflection *reflector;
		D3DReflect(CScode, codeLen, IID_ID3D11ShaderReflection, (void **)&reflector);
		D3D11_SHADER_DESC shaderDesc;
		reflector->GetDesc(&shaderDesc);
		D3D11_SHADER_INPUT_BIND_DESC bdec;
		for (int i = 0; i < shaderDesc.BoundResources; i++)
		{
			reflector->GetResourceBindingDesc(i, &bdec);
			if (bdec.Type == D3D_SIT_STRUCTURED)
			{

			}

		}


		ReflectStage(reflector);
		reflector->Release();
	}

	void ComputeShader::Shutdown()
	{
		mComputeShader->Release();
	}

	void ComputeShader::Run()
	{
		mGDI->GetDeviceContext()->CSSetShader(mComputeShader, nullptr, 0);
		
		mGDI->GetDeviceContext()->Dispatch(32, 1, 1);

		D3D11_QUERY_DESC pQueryDesc;
		pQueryDesc.Query = D3D11_QUERY_EVENT;
		pQueryDesc.MiscFlags = 0;
		ID3D11Query *pEventQuery;
		mGDI->GetDevice()->CreateQuery(&pQueryDesc, &pEventQuery);
		mGDI->GetDeviceContext()->End(pEventQuery);
		while (mGDI->GetDeviceContext()->GetData(pEventQuery, NULL, 0, 0) == S_FALSE) {} 
		pEventQuery->Release();
		
	}
	void PixelShader::Initialize(GDI * gdi, const wchar_t * PSname)
	{
		ID3DBlob* pErrorBlob = nullptr;
		ID3DBlob* pPixelBlob = nullptr;

		HRESULT hr = D3DCompileFromFile(PSname, NULL,
			NULL, "PS", "ps_4_0", 0, 0, &pPixelBlob, &pErrorBlob);
		if (FAILED(hr))
			if (pErrorBlob == nullptr)
				XGF_ReportError("Pixel Shader File Not Find!", PSname);
			else
				XGF_Error_Check(hr, (LPCSTR)pErrorBlob->GetBufferPointer());
		Initialize(gdi, (unsigned char *)pPixelBlob->GetBufferPointer(), pPixelBlob->GetBufferSize());
		pPixelBlob->Release();
	}

	void PixelShader::Initialize(GDI * gdi, const unsigned char * PScode, unsigned int codeLen)
	{
		XGF_Error_Check(gdi->GetDevice()->CreatePixelShader(PScode,
			codeLen, NULL, &mPixelShader), "CreatePixelShader Failed");
		PutDebugString(mPixelShader);
		ID3D11ShaderReflection *reflector;
		D3DReflect(PScode, codeLen, IID_ID3D11ShaderReflection, (void **)&reflector);
		ReflectStage(reflector);
		reflector->Release();
		mGDI = gdi;
	}

	void PixelShader::Shutdown()
	{
		mPixelShader->Release();
	}

	void VertexShader::Initialize(GDI * gdi, const wchar_t * VSname)
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
		Initialize(gdi, (unsigned char *)pVertexBlob->GetBufferPointer(), (int)pVertexBlob->GetBufferSize());
		
		pVertexBlob->Release();
	}

	void VertexShader::Initialize(GDI * gdi, const unsigned char * VScode, unsigned int codeLen)
	{
		XGF_Error_Check(gdi->GetDevice()->CreateVertexShader(VScode,
			codeLen, NULL, &mVertexShader), "CreateVertexShader Failed");
		PutDebugString(mVertexShader);
		mGDI = gdi;
		ID3D11ShaderReflection *reflector;
		D3DReflect(VScode, codeLen, IID_ID3D11ShaderReflection, (void **)&reflector);
		D3D11_SIGNATURE_PARAMETER_DESC desc;
		int i = 0;
		D3D11_SHADER_DESC ShaderDesc;
		reflector->GetDesc(&ShaderDesc);
		int layoutCount = ShaderDesc.InputParameters;
		D3D11_INPUT_ELEMENT_DESC *d3dlayout = new D3D11_INPUT_ELEMENT_DESC[layoutCount];
		mSlotStride = new unsigned int[layoutCount];
		for(int i = 0 ; i < layoutCount; i++)
		{
			reflector->GetInputParameterDesc(i, &desc);
			d3dlayout[i].SemanticName = desc.SemanticName;
			d3dlayout[i].SemanticIndex = desc.SemanticIndex;
			d3dlayout[i].InputSlot = 0;
			d3dlayout[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			d3dlayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			d3dlayout[i].InstanceDataStepRate = 0;

			if (desc.Mask == 1)
			{
				mSlotStride[i] = 4u;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) d3dlayout[i].Format = DXGI_FORMAT_R32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) d3dlayout[i].Format = DXGI_FORMAT_R32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) d3dlayout[i].Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (desc.Mask <= 3)
			{
				mSlotStride[i] = 8u;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) d3dlayout[i].Format = DXGI_FORMAT_R32G32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) d3dlayout[i].Format = DXGI_FORMAT_R32G32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) d3dlayout[i].Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (desc.Mask <= 7)
			{
				mSlotStride[i] = 12u;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) d3dlayout[i].Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) d3dlayout[i].Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) d3dlayout[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (desc.Mask <= 15)
			{
				mSlotStride[i] = 16u;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) d3dlayout[i].Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) d3dlayout[i].Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) d3dlayout[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
		}
		XGF_Error_Check(gdi->GetDevice()->CreateInputLayout(d3dlayout, layoutCount,
			VScode, codeLen, &mInputLayout), "CreateInputLayout Error");
		delete[] d3dlayout;
		ReflectStage(reflector);
		reflector->Release();
		mCount = layoutCount;
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


	unsigned int * VertexShader::GetStride()
	{
		return mSlotStride;
	}

	unsigned int VertexShader::GetStrideAllSize()
	{
		unsigned j = 0;
		for (int i = 0; i < mCount; i++)
			j += mSlotStride[i];
		return j;
	}


	void GeometryShader::Initialize(GDI * gdi, const wchar_t * GSname, bool streamOut)
	{
		ID3DBlob* pErrorBlob = nullptr;
		ID3DBlob* pGeometryBlob = nullptr;
		

		HRESULT hr = D3DCompileFromFile(GSname, NULL,
			NULL, "GS", "gs_4_0", 0, 0, &pGeometryBlob, &pErrorBlob);
		if (FAILED(hr))
			if (pErrorBlob == nullptr)
				XGF_ReportError("Geometry Shader File Not Find!", GSname);
			else
				XGF_Error_Check(hr, (LPCSTR)pErrorBlob->GetBufferPointer());
		Initialize(gdi,(unsigned char *)pGeometryBlob->GetBufferPointer(), pGeometryBlob->GetBufferSize(), streamOut);
		pGeometryBlob->Release();
	}

	void GeometryShader::Initialize(GDI * gdi, const unsigned char * GScode, unsigned int codeLen, bool streamOut)
	{
		mGDI = gdi;
		mStreamOut = streamOut;
		ID3D11ShaderReflection *reflector;
		D3DReflect(GScode, codeLen, IID_ID3D11ShaderReflection, (void **)&reflector);
		if(!streamOut)
		{
			XGF_Error_Check(gdi->GetDevice()->CreateGeometryShader(GScode,
				codeLen, NULL, &mGeometryShader), "CreateGeometryShader Failed");
			mOutBuffer[0] = nullptr;
			mOutBuffer[1] = nullptr;
		}
		
		else
		{
			mPrimitive = reflector->GetGSInputPrimitive();
			D3D11_SHADER_DESC ShaderDesc;
			reflector->GetDesc(&ShaderDesc);
			int emCount = ShaderDesc.OutputParameters;
			D3D11_SIGNATURE_PARAMETER_DESC desc;
			
			D3D11_SO_DECLARATION_ENTRY so[16];
			unsigned int isr[D3D11_SO_BUFFER_SLOT_COUNT] = {0};
			int i = 0;
			for (;i < emCount; i++)
			{
				reflector->GetOutputParameterDesc(i, &desc);
				so[i].SemanticName = desc.SemanticName;
				so[i].SemanticIndex = desc.SemanticIndex;
				so[i].Stream = desc.Stream;
				so[i].StartComponent = 0;
				if (desc.Mask == 1)
				{
					isr[i] = 4u;
					so[i].ComponentCount = 1;
				}
				else if (desc.Mask <= 3)
				{
					isr[i] = 8u;
					so[i].ComponentCount = 2;
				}
				else if (desc.Mask <= 7)
				{
					isr[i] = 12u;
					so[i].ComponentCount = 3;
				}
				else if (desc.Mask <= 15)
				{
					isr[i] = 16u;
					so[i].ComponentCount = 4;
				}
				so[i].OutputSlot = 0;
			}
			XGF_Error_Check(gdi->GetDevice()->CreateGeometryShaderWithStreamOutput(GScode,
					codeLen, so, i, isr, 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &mGeometryShader), "CreateGeometryShader Failed");
			
			D3D11_BUFFER_DESC bufferDesc =
			{
				10000,//TODO::max_vec_count
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER,
				0,
				0,
				0
			};
			gdi->GetDevice()->CreateBuffer(&bufferDesc, NULL, &mOutBuffer[0]);
			gdi->GetDevice()->CreateBuffer(&bufferDesc, NULL, &mOutBuffer[1]);
		}
		PutDebugString(mGeometryShader);
		ReflectStage(reflector);
		reflector->Release();
	}

	void GeometryShader::Swap()
	{
		std::swap(mOutBuffer[0], mOutBuffer[1]);
	}

	void GeometryShader::Shutdown()
	{
		if (mOutBuffer[0] != nullptr)
			mOutBuffer[0]->Release();
		if (mOutBuffer[1] != nullptr)
			mOutBuffer[1]->Release();
		mGeometryShader->Release();
	}


	void ShaderStage::Initialize(VertexShader * vs, PixelShader * ps, GeometryShader * gs)
	{
		this->vs = vs;
		this->gs = gs;
		this->ps = ps;
		mDepthStencilState = DepthStencilState::DepthEnable;
		mBlendState = BlendState::NoneBlend;
		auto init = [](Shader * shader, ConstantBuffer ** cb)
		{
			if (shader->GetCBufferCount() > 0)
			{
				*cb = new ConstantBuffer[shader->GetCBufferCount()];
				for (int i = 0; i < shader->GetCBufferCount(); i++)
					(*cb)[i].Initialize(shader->GetGDI(), shader->GetCBufferSize(i));
			}
			else
				*cb = nullptr;
		};
		if (vs != nullptr)
			init(vs, &mVSCBuffer);
		else
			mVSCBuffer = nullptr;
		if (ps != nullptr)
			init(ps, &mPSCBuffer);
		else
			mPSCBuffer = nullptr;

		if (gs != nullptr)
			init(gs, &mGSCBuffer);
		else
			mGSCBuffer = nullptr;
		if (vs->GetTextureCount() > 0)
		{
			mVSSRV = new ShaderResourceView[vs->GetTextureCount()];
			memset(mVSSRV, 0, sizeof(ShaderResourceView) * vs->GetTextureCount());
		}
		else
			mVSSRV = nullptr;
		if (ps != nullptr && ps->GetTextureCount() > 0)
		{
			mPSSRV = new ShaderResourceView[ps->GetTextureCount()];
			memset(mPSSRV, 0, sizeof(ShaderResourceView) * ps->GetTextureCount());
		}
		else
			mPSSRV = nullptr;
		if (gs != nullptr && gs->GetTextureCount() > 0)
		{
			mGSSRV = new ShaderResourceView[gs->GetTextureCount()];
			memset(mGSSRV, 0, sizeof(ShaderResourceView) * gs->GetTextureCount());
		}
		else
			mGSSRV = nullptr;
		auto initSampler = [](Shader * shader, SamplerState ** ss)
		{
			if (shader->GetSamplerStateCount() > 0)
			{
				*ss = new SamplerState[shader->GetSamplerStateCount()];
				for (int i = 0; i < shader->GetSamplerStateCount(); i++)
					*ss[i] = SamplerState::LineWrap;
			}
			else
				*ss = nullptr;

		};
		if (vs != nullptr && vs->GetSamplerStateCount() > 0)
			initSampler(vs, &mVSSamplerState);
		else
			mVSSamplerState = nullptr;
		if (ps != nullptr && ps->GetSamplerStateCount() > 0)
			initSampler(ps, &mPSSamplerState);
		else
			mPSSamplerState = nullptr;
		if (gs != nullptr && gs->GetSamplerStateCount() > 0)
			initSampler(gs, &mGSSamplerState);
		else
			mGSSamplerState = nullptr;

	}

	void ShaderStage::Shutdown()
	{
		auto shutdown = [](Shader * shader, ConstantBuffer ** cb)
		{
			if (shader->GetCBufferCount() > 0)
			{
				for (int i = 0; i < shader->GetCBufferCount(); i++)
					(*cb)[i].Shutdown();
				delete[] *cb;
			}
		};
		auto shutdownSampler = [](Shader * shader, SamplerState ** ss)
		{
			if (shader->GetSamplerStateCount() > 0)
				delete[] * ss;
		};
		if (mVSCBuffer != nullptr)
			shutdown(vs, &mVSCBuffer);
		if (mPSCBuffer != nullptr)
			shutdown(ps, &mPSCBuffer);
		if (mGSCBuffer != nullptr)
			shutdown(gs, &mGSCBuffer);
		if (mGSSRV)
			delete[] mGSSRV;
		if (mVSSRV)
			delete[] mVSSRV;
		if (mPSSRV)
			delete[] mPSSRV;
		if (mVSSamplerState != nullptr)
			shutdownSampler(vs, &mVSSamplerState);
		if (mPSSamplerState != nullptr)
			shutdownSampler(ps, &mPSSamplerState);
		if (mGSSamplerState != nullptr)
			shutdownSampler(gs, &mGSSamplerState);
	}

	void ShaderStage::SetBlendState(BlendState bs)
	{
		mBlendState = bs;
	}

	void ShaderStage::SetDepthStencilState(DepthStencilState ds)
	{
		mDepthStencilState = ds;
	}

	BlendState ShaderStage::GetBlendState()
	{
		return mBlendState;
	}

	void ShaderStage::SetVSConstantBuffer(int index, const void * data)
	{
		if (index < vs->GetCBufferCount())
			memcpy(mVSCBuffer[index].GetBufferPoint(), data, mVSCBuffer[index].GetSize());
		else
			XGF_ReportWarn0("index out of range in GS ContantBuffer");
	}

	void ShaderStage::SetGSConstantBuffer(int index, const void * data)
	{
		if (index < gs->GetCBufferCount())
			memcpy(mGSCBuffer[index].GetBufferPoint(), data, mGSCBuffer[index].GetSize());
		else
			XGF_ReportWarn0("index out of range in GS ContantBuffer");
	}

	void ShaderStage::SetPSConstantBuffer(int index, const void * data)
	{
		if (index < ps->GetCBufferCount())
			memcpy(mPSCBuffer[index].GetBufferPoint(), data, mPSCBuffer[index].GetSize());
		else
			XGF_ReportWarn0("index out of range in PS ContantBuffer");
	}

	void ShaderStage::SetVSSamplerState(int index, SamplerState ss)
	{
		mVSSamplerState[index] = ss;
	}

	void ShaderStage::SetPSSamplerState(int index, SamplerState ss)
	{
		mPSSamplerState[index] = ss;
	}

	void ShaderStage::SetGSSamplerState(int index, SamplerState ss)
	{
		mGSSamplerState[index] = ss;
	}

	void ShaderStage::SetVSSRV(int index, ID3D11ShaderResourceView * srv)
	{
		mVSSRV[index].srv = srv;
	}
	void ShaderStage::SetPSSRV(int index, ID3D11ShaderResourceView * srv)
	{
		mPSSRV[index].srv = srv;
	}
	
	void ShaderStage::SetGSSRV(int slot, ID3D11ShaderResourceView * srv)
	{
		mGSSRV[slot].srv = srv;
	}
	

	void ShaderStage::BindStage()
	{
		auto dev = vs->mGDI->GetDeviceContext();
		dev->VSSetShader(vs->mVertexShader, 0, 0);
		if(ps != nullptr)
			dev->PSSetShader(ps->mPixelShader, 0, 0);
		if(gs != nullptr)
			dev->GSSetShader(gs->mGeometryShader, 0, 0);
		dev->IASetInputLayout(vs->mInputLayout);
		if (mVSCBuffer != nullptr)
		{
			ID3D11Buffer * buf;
			for (int i = 0; i < vs->mCBufferCount; i++)
			{
				mVSCBuffer[i].MapToGPU(vs->mGDI);
				buf = mVSCBuffer[i].GetRawBuffer();
				dev->VSSetConstantBuffers(vs->GetCBufferSlot(i), 1, &buf);
			}
		}
		if (mPSCBuffer != nullptr)
		{
			ID3D11Buffer * buf;
			for (int i = 0; i < ps->mCBufferCount; i++)
			{
				mPSCBuffer[i].MapToGPU(vs->mGDI);
				buf = mPSCBuffer[i].GetRawBuffer();
				dev->PSSetConstantBuffers(ps->GetCBufferSlot(i), 1, &buf);
			}
		}
		if (mGSCBuffer != nullptr)
		{
			ID3D11Buffer * buf;
			for (int i = 0; i < gs->mCBufferCount; i++)
			{
				mGSCBuffer[i].MapToGPU(gs->mGDI);
				buf = mGSCBuffer[i].GetRawBuffer();
				dev->GSSetConstantBuffers(gs->GetCBufferSlot(i), 1, &buf);
			}
		}
		if (mVSSRV != nullptr)
			for (int i = 0; i < vs->GetTextureCount(); i++)
				if (mVSSRV[i].srv != nullptr)
					dev->VSSetShaderResources(vs->GetTextureSlot(i), 1, &mVSSRV[i].srv);
				else { XGF_ReportWarn0("invalid srv in vs"); }
		if (mPSSRV != nullptr)
			for (int i = 0; i < ps->GetTextureCount(); i++)
				if (mPSSRV[i].srv != nullptr)
					dev->PSSetShaderResources(ps->GetTextureSlot(i), 1, &mPSSRV[i].srv);
				else { XGF_ReportWarn0("invalid srv in ps"); }
		if (mGSSRV != nullptr)
			for (int i = 0; i < gs->GetTextureCount(); i++)
				if(mGSSRV[i].srv != nullptr)
					dev->GSSetShaderResources(gs->GetTextureSlot(i), 1, &mGSSRV[i].srv);
				else { XGF_ReportWarn0("invalid srv in gs"); }
		vs->GetGDI()->SetBlendState(mBlendState);

		ID3D11SamplerState *sps;
		if (mVSSamplerState != nullptr)
			for (int i = 0; i < vs->GetSamplerStateCount(); i++)	
			{
				sps = vs->mGDI->GetRawSamplerState(mVSSamplerState[i]);
				dev->VSSetSamplers(vs->GetSamplerStateSlot(i), 1, &sps);
			}
		if (mPSSamplerState != nullptr)
			for (int i = 0; i < ps->GetSamplerStateCount(); i++)
			{
				sps = vs->mGDI->GetRawSamplerState(mPSSamplerState[i]);
				dev->PSSetSamplers(ps->GetSamplerStateSlot(i), 1, &sps);
			}
		if (mGSSamplerState != nullptr)
			for (int i = 0; i < gs->GetSamplerStateCount(); i++)
			{
				sps = gs->mGDI->GetRawSamplerState(mGSSamplerState[i]);
				dev->GSSetSamplers(gs->GetSamplerStateSlot(i), 1, &sps);
			}
		vs->GetGDI()->SetDepthStencilState(mDepthStencilState);
		//if (vs->mCBufferCount > 0)
			//dev->VSSetConstantBuffers(0, vs->mCBufferCount, vs->mCBuffers);
	}

	void ShaderStage::UnBindStage()
	{
		void* null = 0 ;
		auto dev = vs->mGDI->GetDeviceContext();
		dev->VSSetShader((ID3D11VertexShader *)null, 0, 0);
		if (ps != nullptr)
			dev->PSSetShader((ID3D11PixelShader *)null, 0, 0);
		if (gs != nullptr)
			dev->GSSetShader((ID3D11GeometryShader *)null, 0, 0);
	}

	void ComputeGPU::Initialize(ComputeShader * cs)
	{

		this->cs = cs;
	}

	void ComputeGPU::Shutdown()
	{
	}
	void ComputeGPU::SetCSSRV(int slot, ID3D11ShaderResourceView * srv)
	{
		mCSSRV[slot].srv = srv;
	}
	void ComputeGPU::SetCSUAV(int slot, ID3D11UnorderedAccessView * uav)
	{
		mCSUAV[slot].uav = uav;
	}
}
