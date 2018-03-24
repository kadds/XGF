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
		
	}
	void Shader::ReflectStage(ID3D11ShaderReflection * reflector)
	{
		D3D11_SHADER_DESC shaderDesc;
		reflector->GetDesc(&shaderDesc);
		//constant Buffer
		int constantBufferCount = shaderDesc.ConstantBuffers;

		if (constantBufferCount > 0)
		{
			ID3D11ShaderReflectionConstantBuffer * cbuffer;
			D3D11_SHADER_BUFFER_DESC shaderBuferDesc;
			for (int i = 0; i < constantBufferCount; i++)
			{
				cbuffer = reflector->GetConstantBufferByIndex(i);
				cbuffer->GetDesc(&shaderBuferDesc);
				
				if (shaderBuferDesc.Type == _D3D_CBUFFER_TYPE::D3D11_CT_CBUFFER || shaderBuferDesc.Type == _D3D_CBUFFER_TYPE::D3D11_CT_TBUFFER)
				{
					CBufferInfo cbi{ shaderBuferDesc.Name, 0, shaderBuferDesc.Size };
					mCBufferInfo.push_back(cbi);
				}
			}
		}
		//parse all bind info
		D3D11_SHADER_INPUT_BIND_DESC bdec;
		for (int i = 0; i < shaderDesc.BoundResources; i++)
		{
			reflector->GetResourceBindingDesc(i, &bdec);
			if (bdec.Type == _D3D_CBUFFER_TYPE::D3D11_CT_CBUFFER || bdec.Type == _D3D_CBUFFER_TYPE::D3D11_CT_TBUFFER)
			{
				for (int i = 0; i < constantBufferCount; i++)
				{
					if (strcmp(mCBufferInfo[i].name.c_str(), bdec.Name) == 0)
					{
						mCBufferInfo[i].slot = bdec.BindPoint;
						break;
					}
				}

			}
			else if (bdec.Type == D3D_SIT_TEXTURE || bdec.Type == D3D_SIT_STRUCTURED)
			{
				TextureInfo tti{ bdec.Name, bdec.BindPoint };
				mTexture2D.push_back(tti);
			}
			else if (bdec.Type == D3D_SIT_SAMPLER)
			{
				SamplerInfo ssi{ bdec.Name, bdec.BindPoint };
				mSamplerState.push_back(ssi);
			}
			else if (bdec.Type == D3D11_SIT_UAV_APPEND_STRUCTURED || bdec.Type == D3D11_SIT_UAV_CONSUME_STRUCTURED || bdec.Type == D3D11_SIT_UAV_RWBYTEADDRESS 
				|| bdec.Type == D3D11_SIT_UAV_RWSTRUCTURED || bdec.Type == D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER || bdec.Type == D3D11_SIT_UAV_RWTYPED)
			{
				UAVInfo uav{ bdec.Name, bdec.BindPoint , bdec.Type, bdec.NumSamples};
				mUAVInfo.push_back(uav);
			}
		}

		
	}
	unsigned int Shader::GetCBufferSize(unsigned int index)
	{
		return mCBufferInfo[index].size;
	}
	unsigned int Shader::GetCBufferCount()
	{
		return mCBufferInfo.size();
	}
	GDI * Shader::GetGDI()
	{
		return mGDI;
	}

	unsigned int Shader::GetCBufferSlot(unsigned int index)
	{
		return mCBufferInfo[index].slot;
	}

	int Shader::GetSamplerStateCount()
	{
		return mSamplerState.size();
	}

	unsigned int Shader::GetSamplerStateSlot(unsigned int index)
	{
		return mSamplerState[index].slot;
	}

	int Shader::GetTextureCount()
	{
		return mTexture2D.size();
	}

	unsigned int Shader::GetTextureSlot(unsigned int index)
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
	}

	void ComputeShader::Initialize(GDI * gdi, const unsigned char * CScode, unsigned int codeLen)
	{
		mGDI = gdi;
		XGF_Error_Check(gdi->GetDevice()->CreateComputeShader(CScode,
			codeLen, nullptr, &mComputeShader), "");
		ID3D11ShaderReflection *reflector;
		D3DReflect(CScode, codeLen, IID_ID3D11ShaderReflection, (void **)&reflector);
		ReflectStage(reflector);
		reflector->Release();
		PutDebugString(mComputeShader);
	}

	void ComputeShader::Shutdown()
	{
		mComputeShader->Release();
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
	void InitializeConstantBuffer(Shader * shader, std::vector<ConstantBuffer> & cb)
	{
		if (shader != nullptr && shader->GetCBufferCount() > 0)
		{
			for (int i = 0; i < shader->GetCBufferCount(); i++)
			{
				ConstantBuffer cbb;
				cbb.Initialize(shader->GetGDI(), shader->GetCBufferSize(i));
				cb.push_back(cbb);
			}
				
		}
	};
	void ShutdownConstantBuffer(Shader * shader, std::vector<ConstantBuffer> & cb)
	{
		for (auto it : cb)
		{
			it.Shutdown();
		}
	};
	void ShutdownSampler(Shader * shader, std::vector<SamplerState> & ss)
	{
		ss.clear();
	};
	void InitializeSampler(Shader * shader, std::vector<SamplerState> & ss)
	{
		if (shader != nullptr && shader->GetSamplerStateCount() > 0)
		{
			for (int i = 0; i < shader->GetSamplerStateCount(); i++)
				ss.push_back(SamplerState::LineWrap);
		}
	};
	void ShaderStage::Initialize(VertexShader * vs, PixelShader * ps, GeometryShader * gs)
	{
		this->vs = vs;
		this->gs = gs;
		this->ps = ps;
		mRasterizerState = RasterizerState::SolidAndCutBack;
		mDepthStencilState = DepthStencilState::DepthEnable;
		mBlendState = BlendState::NoneBlend;
		InitializeConstantBuffer(vs, mVSCBuffer);
		InitializeConstantBuffer(ps, mPSCBuffer);
		InitializeConstantBuffer(gs, mGSCBuffer);
		ShaderResourceView srv;
		srv.srv = nullptr;
		if (vs->GetTextureCount() > 0)
			for(int i = 0; i < vs->GetTextureCount();  i ++)
				mVSSRV.push_back(srv);
		if (ps != nullptr && ps->GetTextureCount() > 0)
			for (int i = 0; i < ps->GetTextureCount(); i++)
				mPSSRV.push_back(srv);
		if (gs != nullptr && gs->GetTextureCount() > 0)
			for (int i = 0; i < gs->GetTextureCount(); i++)
				mGSSRV.push_back(srv);
		InitializeSampler(vs, mVSSamplerState);
		InitializeSampler(ps, mPSSamplerState);
		InitializeSampler(gs, mGSSamplerState);
	}

	void ShaderStage::Shutdown()
	{
		ShutdownConstantBuffer(vs, mVSCBuffer);
		ShutdownConstantBuffer(ps, mPSCBuffer);
		ShutdownConstantBuffer(gs, mGSCBuffer);
		
		ShutdownSampler(vs, mVSSamplerState);
		ShutdownSampler(ps, mPSSamplerState);
		ShutdownSampler(gs, mGSSamplerState);
	}

	void ShaderStage::SetBlendState(BlendState bs)
	{
		mBlendState = bs;
	}

	void ShaderStage::SetDepthStencilState(DepthStencilState ds)
	{
		mDepthStencilState = ds;
	}

	void ShaderStage::SetRasterizerState(RasterizerState rs)
	{
		mRasterizerState = rs;
	}

	BlendState ShaderStage::GetBlendState()
	{
		return mBlendState;
	}

	void ShaderStage::SetVSConstantBuffer(unsigned int index, const void * data)
	{
		if (index < vs->GetCBufferCount())
			memcpy(mVSCBuffer[index].GetBufferPoint(), data, mVSCBuffer[index].GetSize());
		else
			XGF_ReportWarn0("index out of range in GS ContantBuffer");
	}

	void ShaderStage::SetGSConstantBuffer(unsigned int index, const void * data)
	{
		if (index < gs->GetCBufferCount())
			memcpy(mGSCBuffer[index].GetBufferPoint(), data, mGSCBuffer[index].GetSize());
		else
			XGF_ReportWarn0("index out of range in GS ContantBuffer");
	}

	void ShaderStage::SetPSConstantBuffer(unsigned int index, const void * data)
	{
		if (index < ps->GetCBufferCount())
			memcpy(mPSCBuffer[index].GetBufferPoint(), data, mPSCBuffer[index].GetSize());
		else
			XGF_ReportWarn0("index out of range in PS ContantBuffer");
	}


	void ShaderStage::SetVSSamplerState(unsigned int index, SamplerState ss)
	{
		XGF_ASSERT(index < vs->mSamplerState.size());
		if (mVSSamplerState[index] != ss)
			mOnFlush();
		mVSSamplerState[index] = ss;
	}

	void ShaderStage::SetPSSamplerState(unsigned int index, SamplerState ss)
	{
		XGF_ASSERT(index < ps->mSamplerState.size());
		if (mPSSamplerState[index] != ss)
			mOnFlush();
		mPSSamplerState[index] = ss;
	}

	void ShaderStage::SetGSSamplerState(unsigned int index, SamplerState ss)
	{
		XGF_ASSERT(index < gs->mSamplerState.size());
		if (mGSSamplerState[index] != ss)
			mOnFlush();
		mGSSamplerState[index] = ss;
	}

	void ShaderStage::SetVSSRV(unsigned int index, ID3D11ShaderResourceView * srv)
	{
		XGF_ASSERT(index < vs->mTexture2D.size());
		if (mVSSRV[index].srv != srv)
			mOnFlush();
		mVSSRV[index].srv = srv;
	}
	void ShaderStage::SetPSSRV(unsigned int index, ID3D11ShaderResourceView * srv)
	{
		XGF_ASSERT(index < ps->mTexture2D.size());
		if (mPSSRV[index].srv != srv)
			mOnFlush();
		mPSSRV[index].srv = srv;
	}
	
	void ShaderStage::SetGSSRV(unsigned int index, ID3D11ShaderResourceView * srv)
	{
		XGF_ASSERT(index < gs->mTexture2D.size());
		if (mGSSRV[index].srv != srv)
			mOnFlush();
		mGSSRV[index].srv = srv;
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
		if (mVSCBuffer.size() > 0)
		{
			ID3D11Buffer * buf;
			for (int i = 0; i < vs->mCBufferInfo.size(); i++)
			{
				mVSCBuffer[i].MapToGPU(vs->mGDI);
				buf = mVSCBuffer[i].GetRawBuffer();
				dev->VSSetConstantBuffers(vs->GetCBufferSlot(i), 1, &buf);
			}
		}
		if (mPSCBuffer.size() > 0)
		{
			ID3D11Buffer * buf;
			for (int i = 0; i < ps->mCBufferInfo.size(); i++)
			{
				mPSCBuffer[i].MapToGPU(vs->mGDI);
				buf = mPSCBuffer[i].GetRawBuffer();
				dev->PSSetConstantBuffers(ps->GetCBufferSlot(i), 1, &buf);
			}
		}
		if (mGSCBuffer.size() > 0)
		{
			ID3D11Buffer * buf;
			for (int i = 0; i < gs->mCBufferInfo.size(); i++)
			{
				mGSCBuffer[i].MapToGPU(gs->mGDI);
				buf = mGSCBuffer[i].GetRawBuffer();
				dev->GSSetConstantBuffers(gs->GetCBufferSlot(i), 1, &buf);
			}
		}
		if (mVSSRV.size() > 0)
			for (int i = 0; i < vs->GetTextureCount(); i++)
				if (mVSSRV[i].srv != nullptr)
					dev->VSSetShaderResources(vs->GetTextureSlot(i), 1, &mVSSRV[i].srv);
				else { XGF_ReportWarn0("invalid srv in vs"); }
		if (mPSSRV.size() > 0)
			for (int i = 0; i < ps->GetTextureCount(); i++)
				if (mPSSRV[i].srv != nullptr)
					dev->PSSetShaderResources(ps->GetTextureSlot(i), 1, &mPSSRV[i].srv);
				else { XGF_ReportWarn0("invalid srv in ps"); }
		if (mGSSRV.size() > 0)
			for (int i = 0; i < gs->GetTextureCount(); i++)
				if(mGSSRV[i].srv != nullptr)
					dev->GSSetShaderResources(gs->GetTextureSlot(i), 1, &mGSSRV[i].srv);
				else { XGF_ReportWarn0("invalid srv in gs"); }
		vs->GetGDI()->SetBlendState(mBlendState);

		ID3D11SamplerState *sps;
		if (mVSSamplerState.size() > 0)
			for (int i = 0; i < vs->GetSamplerStateCount(); i++)	
			{
				sps = vs->mGDI->GetRawSamplerState(mVSSamplerState[i]);
				dev->VSSetSamplers(vs->GetSamplerStateSlot(i), 1, &sps);
			}
		if (mPSSamplerState.size() > 0)
			for (int i = 0; i < ps->GetSamplerStateCount(); i++)
			{
				sps = vs->mGDI->GetRawSamplerState(mPSSamplerState[i]);
				dev->PSSetSamplers(ps->GetSamplerStateSlot(i), 1, &sps);
			}
		if (mGSSamplerState.size() > 0)
			for (int i = 0; i < gs->GetSamplerStateCount(); i++)
			{
				sps = gs->mGDI->GetRawSamplerState(mGSSamplerState[i]);
				dev->GSSetSamplers(gs->GetSamplerStateSlot(i), 1, &sps);
			}
		vs->GetGDI()->SetDepthStencilState(mDepthStencilState);
		vs->GetGDI()->SetRasterizerState(mRasterizerState);
	}

	void ShaderStage::UnBindStage()
	{
		void* null = 0 ;
		auto dev = vs->mGDI->GetDeviceContext();
		dev->VSSetShader((ID3D11VertexShader *)null, 0, 0);
		if (mVSSRV.size() > 0)
			for (int i = 0; i < vs->GetTextureCount(); i++)
				if (mVSSRV[i].srv != nullptr)
					dev->VSSetShaderResources(vs->GetTextureSlot(i), 1, (ID3D11ShaderResourceView **)(&null));

		if (mPSSRV.size() > 0)
			for (int i = 0; i < ps->GetTextureCount(); i++)
				dev->PSSetShaderResources(ps->GetTextureSlot(i), 1, (ID3D11ShaderResourceView **)(&null));
		if (mGSSRV.size() > 0)
			for (int i = 0; i < gs->GetTextureCount(); i++)
				dev->GSSetShaderResources(gs->GetTextureSlot(i), 1, (ID3D11ShaderResourceView **)(&null));

		if (ps != nullptr)
			dev->PSSetShader((ID3D11PixelShader *)null, 0, 0);
		if (gs != nullptr)
			dev->GSSetShader((ID3D11GeometryShader *)null, 0, 0);

	}

	void ComputeGPU::Initialize(ComputeShader * cs, unsigned int buffersize)
	{
		this->cs = cs;
		if (cs->mUAVInfo.size() > 0)
		{
			for (int i = 0; i < cs->mUAVInfo.size(); i++)
			{
				UnorderedAccessView uav;
				if (cs->mUAVInfo[i].flags == D3D_SIT_UAV_APPEND_STRUCTURED)
				{
					uav.buffer = CreateBuffer(cs->mGDI, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, cs->mUAVInfo[i].size * buffersize, cs->mUAVInfo[i].size, D3D11_USAGE_DEFAULT, (D3D11_CPU_ACCESS_FLAG)0);
					uav.uav = CreateUnorderedAccessView(cs->mGDI, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, D3D11_BUFFER_UAV_FLAG_APPEND);
					uav.srv = CreateShaderResourceView(cs->mGDI, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, (D3D11_BUFFEREX_SRV_FLAG)0);
					mCSUAV.push_back(uav);
				}
				if (cs->mUAVInfo[i].flags == D3D_SIT_UAV_CONSUME_STRUCTURED)
				{
					uav.buffer = CreateBuffer(cs->mGDI, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, cs->mUAVInfo[i].size * buffersize, cs->mUAVInfo[i].size, D3D11_USAGE_DEFAULT, (D3D11_CPU_ACCESS_FLAG)0);
					uav.uav = CreateUnorderedAccessView(cs->mGDI, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, D3D11_BUFFER_UAV_FLAG_APPEND);
					uav.srv = CreateShaderResourceView(cs->mGDI, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, (D3D11_BUFFEREX_SRV_FLAG)0);
					mCSUAV.push_back(uav);
				}
				if (cs->mUAVInfo[i].flags == D3D_SIT_UAV_RWSTRUCTURED)
				{
					uav.buffer = CreateBuffer(cs->mGDI, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, cs->mUAVInfo[i].size * buffersize, cs->mUAVInfo[i].size, D3D11_USAGE_DEFAULT, (D3D11_CPU_ACCESS_FLAG)0);
					uav.uav = CreateUnorderedAccessView(cs->mGDI, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, D3D11_BUFFER_UAV_FLAG(0));
					uav.srv = CreateShaderResourceView(cs->mGDI, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, (D3D11_BUFFEREX_SRV_FLAG)0);
					mCSUAV.push_back(uav);
				}
			}
		}
		InitializeConstantBuffer(cs, mCSCBuffer);
		InitializeSampler(cs, mCSSamplerState);
		ShaderResourceView srv = { nullptr };
		if (cs->GetTextureCount() > 0)
			for (int i = 0; i < cs->GetTextureCount(); i++)
				mCSSRV.push_back(srv);
		
	}
	void ComputeGPU::Shutdown()
	{
		for (auto it : mCSUAV)
		{
			it.buffer->Release();
			it.srv->Release();
			it.uav->Release();
		}
		ShutdownConstantBuffer(cs, mCSCBuffer);
		ShutdownSampler(cs, mCSSamplerState);
	}
	void ComputeGPU::SetCSSRV(unsigned int index, ID3D11ShaderResourceView * srv)
	{
		XGF_ASSERT(index < cs->mTexture2D.size());

		mCSSRV[index].srv = srv;
	}
	void ComputeGPU::SetCSUAV(unsigned int index, ID3D11UnorderedAccessView * uav)
	{
		XGF_ASSERT(index < cs->mUAVInfo.size());
		mCSUAV[index].uav = uav;
	}
	void ComputeGPU::SetCSSamplerState(unsigned int index, SamplerState state)
	{
		mCSSamplerState[index] = state;
	}
	void ComputeGPU::Run(bool asyn)
	{
		auto gdi = cs->GetGDI();
		gdi->GetDeviceContext()->Dispatch(32, 1, 1);
		if (!asyn)//同步等待gpu运行结束
		{
			D3D11_QUERY_DESC pQueryDesc;
			pQueryDesc.Query = D3D11_QUERY_EVENT;
			pQueryDesc.MiscFlags = 0;
			ID3D11Query *pEventQuery;
			gdi->GetDevice()->CreateQuery(&pQueryDesc, &pEventQuery);
			gdi->GetDeviceContext()->End(pEventQuery);
			while (gdi->GetDeviceContext()->GetData(pEventQuery, NULL, 0, 0) == S_FALSE) {}
			pEventQuery->Release();
		}
	}
	void ComputeGPU::Bind()
	{
		auto gdi = cs->GetGDI();
		ID3D11Buffer * cbuffer;
		for (int i = 0; i < cs->mCBufferInfo.size(); i++)
		{
			mCSCBuffer[i].MapToGPU(gdi);
			cbuffer = mCSCBuffer[i].GetRawBuffer();
			gdi->GetDeviceContext()->CSSetConstantBuffers(cs->mCBufferInfo[i].slot, 1, &cbuffer);
		}

		if (mCSSamplerState.size() > 0)
			for (int i = 0; i < cs->GetSamplerStateCount(); i++)
			{
				auto sps = gdi->GetRawSamplerState(mCSSamplerState[i]);
				gdi->GetDeviceContext()->CSSetSamplers(cs->GetSamplerStateSlot(i), 1, &sps);
			}
		if (mCSSRV.size() > 0)
			for (int i = 0; i < cs->GetTextureCount(); i++)
				if (mCSSRV[i].srv != nullptr)
					gdi->GetDeviceContext()->CSSetShaderResources(cs->GetTextureSlot(i), 1, &mCSSRV[i].srv);
				else { XGF_ReportWarn0("invalid srv in cs"); }
		if (mCSUAV.size() > 0)
			for (int i = 0; i < cs->mUAVInfo.size(); i++)
				if (mCSUAV[i].uav != nullptr)
					gdi->GetDeviceContext()->CSSetUnorderedAccessViews(cs->mUAVInfo[i].slot, 1, &(mCSUAV[i].uav), 0);
				else { XGF_ReportWarn0("invalid uav in cs"); }

		gdi->GetDeviceContext()->CSSetShader(cs->GetShader(), nullptr, 0);
				

	}
	void ComputeGPU::UnBind()
	{
		void* null = 0;
		auto gdi = cs->GetGDI();
		gdi->GetDeviceContext()->CSSetShader((ID3D11ComputeShader *)null, nullptr, 0);
		if (mCSSRV.size() > 0)
			for (int i = 0; i < cs->GetTextureCount(); i++)
				gdi->GetDeviceContext()->CSSetShaderResources(cs->GetTextureSlot(i), 1, (ID3D11ShaderResourceView **)(&null));
		if (mCSUAV.size() > 0)
			for (int i = 0; i < cs->mUAVInfo.size(); i++)
				gdi->GetDeviceContext()->CSSetUnorderedAccessViews(cs->mUAVInfo[i].slot, 1, (ID3D11UnorderedAccessView **)(&null), 0);
	}
	UnorderedAccessView * ComputeGPU::GetUnorderedAccessViews(int index)
	{
		XGF_ASSERT(index < cs->mUAVInfo.size());
		return &mCSUAV[index];
	}
	int ComputeGPU::GetUnorderedAccessViewCount()
	{
		return cs->mUAVInfo.size();
	}
}
