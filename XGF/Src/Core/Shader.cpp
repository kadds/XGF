#include "../../Include/Shader.hpp"
#include "../../Include/GDI.hpp"
#include "../../Include/Logger.hpp"
#include <numeric>
#include "../../Include/Context.hpp"
#include "../../Include/Texture.hpp"

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
		const int constantBufferCount = shaderDesc.ConstantBuffers;

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
		for (unsigned int i = 0; i < shaderDesc.BoundResources; i++)
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
				UAVInfo uav{ bdec.Name, bdec.BindPoint , bdec.Type, bdec.NumSamples };
				mUAVInfo.push_back(uav);
			}
		}
		// sort by slot
		std::sort(mCBufferInfo.begin(), mCBufferInfo.end(), [](const CBufferInfo & v1, const CBufferInfo & v2)
		{
			return v1.slot < v2.slot;
		});
		std::sort(mTexture2D.begin(), mTexture2D.end(), [](const TextureInfo & v1, const TextureInfo & v2)
		{
			return v1.slot < v2.slot;
		});
		std::sort(mSamplerState.begin(), mSamplerState.end(), [](const SamplerInfo & v1, const SamplerInfo & v2)
		{
			return v1.slot < v2.slot;
		});
		std::sort(mUAVInfo.begin(), mUAVInfo.end(), [](const UAVInfo & v1, const UAVInfo & v2)
		{
			return v1.slot < v2.slot;
		});

	}
	unsigned int Shader::GetCBufferSize(unsigned int index)
	{
		return mCBufferInfo[index].size;
	}
	unsigned int Shader::GetCBufferCount()
	{
		return static_cast<unsigned int>(mCBufferInfo.size());
	}

	unsigned int Shader::GetCBufferSlot(unsigned int index)
	{
		return mCBufferInfo[index].slot;
	}

	unsigned int Shader::GetSamplerStateCount()
	{
		return static_cast<unsigned int>(mSamplerState.size());
	}

	unsigned int Shader::GetSamplerStateSlot(unsigned int index)
	{
		return mSamplerState[index].slot;
	}

	unsigned int Shader::GetTextureCount()
	{
		return static_cast<unsigned int>(mTexture2D.size());
	}

	unsigned int Shader::GetTextureSlot(unsigned int index)
	{
		return mTexture2D[index].slot;
	}

	std::string Shader::GetName() const
	{
		return mName;
	}

	void Shader::SetName(const std::string& name)
	{
		mName = name;
	}

	const std::string ComputeShader::mEntrypoint = "VS";
	const std::string ComputeShader::mPerfixName = "vs_";

	void ComputeShader::Initialize(const unsigned char * CScode, unsigned int codeLen)
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

		XGF_Error_Check(IO, gdi.GetDevice()->CreateComputeShader(CScode,
			codeLen, nullptr, &mComputeShader), "Create ComputeShader failed");
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

	const std::string& ComputeShader::GetEntrypoint()
	{
		return mEntrypoint;
	}

	const std::string& ComputeShader::GetPrefixName()
	{
		return mPerfixName;
	}

	void PixelShader::Initialize(const unsigned char * PScode, unsigned int codeLen)
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

		XGF_Error_Check(IO, gdi.GetDevice()->CreatePixelShader(PScode,
			codeLen, NULL, &mPixelShader), "Create pixel shader failed");
		PutDebugString(mPixelShader);
		ID3D11ShaderReflection *reflector;
		D3DReflect(PScode, codeLen, IID_ID3D11ShaderReflection, (void **)&reflector);
		ReflectStage(reflector);
		reflector->Release();
	}

	void PixelShader::Shutdown()
	{
		mPixelShader->Release();
	}
	const std::string PixelShader::mEntrypoint = "PS";
	const std::string PixelShader::mPerfixName = "ps_";
	const std::string & PixelShader::GetEntrypoint()
	{
		return mEntrypoint;
	}

	const std::string & PixelShader::GetPrefixName()
	{
		return mPerfixName;
	}

	void VertexShader::Initialize(const unsigned char * VScode, unsigned int codeLen, unsigned int interval)
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

		XGF_Error_Check(IO, gdi.GetDevice()->CreateVertexShader(VScode,
			codeLen, NULL, &mVertexShader), "Create vertex shader failed");
		PutDebugString(mVertexShader);

		ID3D11ShaderReflection *reflector;
		D3DReflect(VScode, codeLen, IID_ID3D11ShaderReflection, (void **)&reflector);
		D3D11_SIGNATURE_PARAMETER_DESC desc;
		D3D11_SHADER_DESC ShaderDesc;
		reflector->GetDesc(&ShaderDesc);
		unsigned int layoutCount = ShaderDesc.InputParameters;
		auto layouts = std::vector < D3D11_INPUT_ELEMENT_DESC>(layoutCount);
		mInputElements.clear();
		int inputSlot = 0;
		mSlotElementStartPosition.push_back(0);
		unsigned int offset = 0u;
		for (unsigned int i = 0; i < layoutCount; i++)
		{
			unsigned int stride = 0;
			reflector->GetInputParameterDesc(i, &desc);
			layouts[i].SemanticName = desc.SemanticName;
			layouts[i].SemanticIndex = desc.SemanticIndex;
			layouts[i].InputSlot = inputSlot;
			layouts[i].AlignedByteOffset = offset;
			layouts[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			layouts[i].InstanceDataStepRate = 0;

			if (desc.Mask == 1)
			{
				stride = 4u;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) layouts[i].Format = DXGI_FORMAT_R32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) layouts[i].Format = DXGI_FORMAT_R32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) layouts[i].Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (desc.Mask <= 3)
			{
				stride = 8u;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) layouts[i].Format = DXGI_FORMAT_R32G32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) layouts[i].Format = DXGI_FORMAT_R32G32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) layouts[i].Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (desc.Mask <= 7)
			{
				stride = 12u;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) layouts[i].Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) layouts[i].Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) layouts[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (desc.Mask <= 15)
			{
				stride = 16u;
				if (desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) layouts[i].Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) layouts[i].Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) layouts[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			else
			{
				XGF_Warn(Shader, "The mask on inputLayout is incorrect");
			}
			inputSlot = interval == 0 ? 0 : (i + 1) / interval;
			offset += stride;
			if (layouts[i].InputSlot == inputSlot - 1 && i != layoutCount - 1)
			{
				mSlotElementStartPosition.push_back(i + 1);
				offset = 0;
			}
			mSlotStride.push_back(stride);
			mInputElements.emplace_back(std::string(desc.SemanticName), inputSlot, desc.SemanticIndex, stride);
		}
		XGF_Error_Check(IO, gdi.GetDevice()->CreateInputLayout(layouts.data(), layoutCount,
			VScode, codeLen, &mInputLayout), "Create InputLayout failed");
		ReflectStage(reflector);
		reflector->Release();
		mLayoutCount = layoutCount;
		PutDebugString(mInputLayout);
	}


	void VertexShader::Shutdown()
	{
		mVertexShader->Release();
		mInputLayout->Release();
		mInputElements.clear();
		mSlotElementStartPosition.clear();
	}



	const unsigned int * VertexShader::GetStrideAtSlot(unsigned int slot) const
	{
		XGF_ASSERT(slot < mSlotElementStartPosition.size());
		return &mSlotStride[mSlotElementStartPosition[slot]];
	}

	const unsigned int * VertexShader::GetSlotElementStartPositionArray() const
	{
		return mSlotElementStartPosition.data();
	}

	unsigned int VertexShader::GetStrideAllSizeAtSlot(unsigned int slot) const
	{
		XGF_ASSERT(slot < mSlotElementStartPosition.size());
		std::vector<unsigned int>::const_iterator end;
		if (slot == mSlotElementStartPosition.size() - 1)
		{
			end = mSlotStride.end();
		}
		else
		{
			end = mSlotStride.begin() + mSlotElementStartPosition[slot + 1];
		}
		return std::accumulate(mSlotStride.begin() + mSlotElementStartPosition[slot], end, 0);
	}

	unsigned int VertexShader::GetSlotCount() const
	{
		return static_cast<unsigned int>(mSlotElementStartPosition.size());
	}
	unsigned int VertexShader::GetInputCount() const
	{
		return static_cast<unsigned int>(mSlotStride.size());
	}

	const unsigned int * VertexShader::GetStride() const
	{
		return mSlotStride.data();
	}

	unsigned int VertexShader::GetInputLayoutCount() const
	{
		return mLayoutCount;
	}

	bool VertexShader::HasSemanticTexcoord()
	{
		return HasSemantic("TEXCOORD");
	}

	bool VertexShader::HasSemanticNormal()
	{
		return HasSemantic("NORMAL");
	}
	const std::string VertexShader::mEntrypoint = "VS";
	const std::string VertexShader::mPerfixName = "vs_";
	bool VertexShader::HasSemantic(const char * name)
	{
		for (auto & it : mInputElements)
		{
			if (it.name == name)
				return true;
		}
		return false;
	}
	const std::string & VertexShader::GetEntrypoint()
	{
		return mEntrypoint;
	}
	const std::string & VertexShader::GetPrefixName()
	{
		return mPerfixName;
	}
	const std::string GeometryShader::mEntrypoint = "VS";
	const std::string GeometryShader::mPerfixName = "vs_";

	VertexShader::InputElement::InputElement(const std::string & name, int index, int slot, unsigned stride) : name(name), index(index), slot(slot),
		stride(stride)
	{
	}

	void GeometryShader::Initialize(const unsigned char * GScode, unsigned int codeLen, bool streamOut)
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

		mStreamOut = streamOut;
		ID3D11ShaderReflection *reflector;
		D3DReflect(GScode, codeLen, IID_ID3D11ShaderReflection, (void **)&reflector);
		if (!streamOut)
		{
			XGF_Error_Check(IO, gdi.GetDevice()->CreateGeometryShader(GScode,
				codeLen, NULL, &mGeometryShader), "Create GeometryShader failed");
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
			unsigned int isr[D3D11_SO_BUFFER_SLOT_COUNT] = { 0 };
			int i = 0;
			for (; i < emCount; i++)
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
			XGF_Error_Check(IO, gdi.GetDevice()->CreateGeometryShaderWithStreamOutput(GScode,
				codeLen, so, i, isr, 1, D3D11_SO_NO_RASTERIZED_STREAM, NULL, &mGeometryShader), "Create GeometryShader failed");

			D3D11_BUFFER_DESC bufferDesc =
			{
				10000,//TODO::max_vec_count
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER,
				0,
				0,
				0
			};
			gdi.GetDevice()->CreateBuffer(&bufferDesc, NULL, &mOutBuffer[0]);
			gdi.GetDevice()->CreateBuffer(&bufferDesc, NULL, &mOutBuffer[1]);
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
	const std::string & GeometryShader::GetEntrypoint()
	{
		return mEntrypoint;
	}
	const std::string & GeometryShader::GetPrefixName()
	{
		return mPerfixName;
	}
	void InitializeConstantBuffer(Shader * shader, std::vector<StageConstantBuffer> & cb)
	{
		if (shader != nullptr && shader->GetCBufferCount() > 0)
		{
			for (unsigned int i = 0; i < shader->GetCBufferCount(); i++)
			{
				cb.emplace_back(shader->GetCBufferSize(i));
			}

		}
	};
	void ShutdownConstantBuffer(std::vector<StageConstantBuffer> & cb)
	{
		cb.clear();
	};
	void ShutdownSampler(std::vector<SamplerState> & ss)
	{
		ss.clear();
	};
	void InitializeSampler(Shader * shader, std::vector<SamplerState> & ss)
	{
		if (shader != nullptr && shader->GetSamplerStateCount() > 0)
		{
			for (unsigned int i = 0; i < shader->GetSamplerStateCount(); i++)
				ss.emplace_back();
		}
	};

	StageConstantBuffer::StageConstantBuffer(unsigned size): size(size)
	{
		ptr = std::shared_ptr<char>(new char[size], std::default_delete<char[]>());
	}

	StageConstantBuffer::StageConstantBuffer(const StageConstantBuffer& scb)
	{
		size = scb.size;
		ptr = std::shared_ptr<char>(new char[size], std::default_delete<char[]>());
		memcpy(ptr.get(), scb.ptr.get(), size * sizeof(char));
	}

	void* StageConstantBuffer::GetBufferPoint() const
	{
		return ptr.get();
	}

	size_t StageConstantBuffer::GetSize() const
	{
		return size;
	}

	RenderResource::RenderResource(Shaders shaders)
	{
		ReCreate(shaders);
	}

	void RenderResource::ReCreate(Shaders shaders)
	{
		mShaders = shaders;
		InitializeConstantBuffer(mShaders.vs, mVSCBuffer);
		InitializeConstantBuffer(mShaders.ps, mPSCBuffer);
		InitializeConstantBuffer(mShaders.gs, mGSCBuffer);

		if (mShaders.vs->GetTextureCount() > 0)
			mVSTexture.resize(mShaders.vs->GetTextureCount());
		if (mShaders.ps != nullptr && mShaders.ps->GetTextureCount() > 0)
			mPSTexture.resize(mShaders.ps->GetTextureCount());
		if (mShaders.gs != nullptr && mShaders.gs->GetTextureCount() > 0)
			mGSTexture.resize(mShaders.gs->GetTextureCount());
		InitializeSampler(mShaders.vs, mVSSamplerState);
		InitializeSampler(mShaders.ps, mPSSamplerState);
		InitializeSampler(mShaders.gs, mGSSamplerState);
	}

	void RenderResource::Clear()
	{
		ShutdownConstantBuffer(mVSCBuffer);
		ShutdownConstantBuffer(mPSCBuffer);
		ShutdownConstantBuffer(mGSCBuffer);

		ShutdownSampler(mVSSamplerState);
		ShutdownSampler(mPSSamplerState);
		ShutdownSampler(mGSSamplerState);
	}

	RenderResource::~RenderResource()
	{
		Clear();
	}


	void RenderResource::CopyToConstantBuffer(std::vector<StageConstantBuffer>& target, Shader* shader, unsigned index,
	                                       int offset, std::pair<const void *, size_t> * ptr, size_t size)
	{
		if (index < shader->GetCBufferCount())
		{
			auto dst = target[index].GetBufferPoint();
			size_t start = static_cast<size_t>(offset);
			for (auto i = 0; i < size; i++)
			{
				auto & value = ptr[i];
				memcpy((char *)dst + start, value.first, value.second);
				start += value.second;
			}
			if(start > target[index].GetSize())
			{
				XGF_Warn(Shader, "The offset in the constant buffer is out of range.", "offset: ", offset);
			}
		}
		else
			XGF_Warn(Shader, "The index in the constant buffer is out of range.");
	}

	void RenderResource::CopyToConstantBuffer(std::vector<StageConstantBuffer>& target, Shader* shader, unsigned index,
	                                       std::function<void(void*, size_t size)> func)
	{
		if (index < shader->GetCBufferCount())
		{
			func(target[index].GetBufferPoint(), target[index].GetSize());
		}
		else
			XGF_Warn(Shader, "The index in the constant buffer is out of range.");
	}

	void RenderResource::SetSamplerState(std::vector<SamplerState>& states, Shader* shader, unsigned index,
	                                  SamplerState ss)
	{
		XGF_ASSERT(index < shader->mSamplerState.size());
		states[index] = ss;
	}

	void RenderResource::SetTexture(std::vector<Texture*>& textures, Shader * shader, unsigned index, Texture* texture)
	{
		XGF_ASSERT(index < shader->mTexture2D.size());
		textures[index] = texture;
	}

	void RawRenderStage::BindStage()
	{
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

		auto dev = gdi.GetDeviceContext();
		dev->VSSetShader(vs->mVertexShader, 0, 0);
		if (ps != nullptr)
			dev->PSSetShader(ps->mPixelShader, 0, 0);
		else
			dev->PSSetShader(0, 0, 0);
		if (gs != nullptr)
			dev->GSSetShader(gs->mGeometryShader, 0, 0);
		else
			dev->GSSetShader(0, 0, 0);
		dev->IASetInputLayout(vs->mInputLayout);
		dev->IASetPrimitiveTopology(mTopologyMode);
		ID3D11ShaderResourceView * tex;
		if (!mVSTexture.empty())
			for (unsigned i = 0; i < vs->GetTextureCount(); i++)
				if (mVSTexture[i] != nullptr)
				{
					mVSTexture[i]->UpdateTexture();
					tex = mVSTexture[i]->GetRawTexture();
					dev->VSSetShaderResources(vs->GetTextureSlot(i), 1, &tex);
				}
				else { XGF_Warn(Shader, "Invalid texture in vs"); }
		if (!mPSTexture.empty())
			for (unsigned i = 0; i < ps->GetTextureCount(); i++)
				if (mPSTexture[i] != nullptr)
				{
					mPSTexture[i]->UpdateTexture();
					tex = mPSTexture[i]->GetRawTexture();
					dev->PSSetShaderResources(ps->GetTextureSlot(i), 1, &tex);
				}
				else { XGF_Warn(Shader, "Invalid texture in ps"); }
		if (!mGSTexture.empty())
			for (unsigned i = 0; i < gs->GetTextureCount(); i++)
				if (mGSTexture[i] != nullptr)
				{
					mGSTexture[i]->UpdateTexture();
					tex = mGSTexture[i]->GetRawTexture();
					dev->GSSetShaderResources(gs->GetTextureSlot(i), 1, &tex);
				}
				else { XGF_Warn(Shader, "Invalid texture in gs"); }
		gdi.SetBlendState(mBlendState);

		ID3D11SamplerState *sps;
		if (!mVSSamplerState.empty())
			for (unsigned i = 0; i < vs->GetSamplerStateCount(); i++)
			{
				sps = mVSSamplerState[i].GetPtr<ID3D11SamplerState>();
				dev->VSSetSamplers(vs->GetSamplerStateSlot(i), 1, &sps);
			}
		if (!mPSSamplerState.empty())
			for (unsigned i = 0; i < ps->GetSamplerStateCount(); i++)
			{
				sps = mPSSamplerState[i].GetPtr<ID3D11SamplerState>();
				dev->PSSetSamplers(ps->GetSamplerStateSlot(i), 1, &sps);
			}
		if (!mGSSamplerState.empty())
			for (unsigned i = 0; i < gs->GetSamplerStateCount(); i++)
			{
				sps = mGSSamplerState[i].GetPtr<ID3D11SamplerState>();
				dev->GSSetSamplers(gs->GetSamplerStateSlot(i), 1, &sps);
			}
		gdi.SetDepthStencilState(mDepthStencilState);
		gdi.SetRasterizerState(mRasterizerState);
		gdi.SetScissorRectangle(mScissorRects);
		gdi.SetViewPorts(mViewPorts);
	}

	void RawRenderStage::UnBindStage()
	{
		void* null = 0;
		auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

		auto dev = gdi.GetDeviceContext();
		dev->VSSetShader(0, 0, 0);
		if (!mVSTexture.empty())
			for (unsigned i = 0; i < vs->GetTextureCount(); i++)
				if (mVSTexture[i] != nullptr)
					dev->VSSetShaderResources(vs->GetTextureSlot(i), 1, (ID3D11ShaderResourceView **)(&null));

		if (!mPSTexture.empty())
			for (unsigned i = 0; i < ps->GetTextureCount(); i++)
				if (mPSTexture[i] != nullptr)
					dev->PSSetShaderResources(ps->GetTextureSlot(i), 1, (ID3D11ShaderResourceView **)(&null));
		if (!mGSTexture.empty())
			for (unsigned i = 0; i < gs->GetTextureCount(); i++)
				if (mGSTexture[i] != nullptr)
					dev->GSSetShaderResources(gs->GetTextureSlot(i), 1, (ID3D11ShaderResourceView **)(&null));

		if (ps != nullptr)
			dev->PSSetShader(0, 0, 0);
		if (gs != nullptr)
			dev->GSSetShader(0, 0, 0);

	}

	RawRenderStage::RawRenderStage(const RenderStage& ss) : mVSSamplerState(ss.GetRenderResource()->GetSamplerState<VertexShader>().size())
		, mPSSamplerState(ss.GetRenderResource()->GetSamplerState<PixelShader>().size())
		, mGSSamplerState(ss.GetRenderResource()->GetSamplerState<GeometryShader>().size())
		, mVSCBuffer(ss.GetRenderResource()->GetConstantBuffer<VertexShader>())
		, mPSCBuffer(ss.GetRenderResource()->GetConstantBuffer<PixelShader>())
		, mGSCBuffer(ss.GetRenderResource()->GetConstantBuffer<GeometryShader>())
		, mVSTexture(ss.GetRenderResource()->GetTexture<VertexShader>())
		, mPSTexture(ss.GetRenderResource()->GetTexture<PixelShader>())
		, mGSTexture(ss.GetRenderResource()->GetTexture<GeometryShader>()),
		mTopologyMode(ss.GetRenderState()->GetTopologyMode())
		, vs(ss.GetRenderResource()->GetShader<VertexShader>())
		, ps(ss.GetRenderResource()->GetShader<PixelShader>())
		, gs(ss.GetRenderResource()->GetShader<GeometryShader>())
		, mScissorRects(ss.GetRenderState()->GetScissorRects())
		, mViewPorts(ss.GetRenderState()->GetViewPorts())
	{
		auto& gdi = Context::Current().QueryGraphicsDeviceInterface();
		mRasterizerState = gdi.GetRasterizerState(ss.GetRenderState()->GetRasterizerState());
		mDepthStencilState = gdi.GetDepthStencilState(ss.GetRenderState()->GetDepthStencilState());
		mBlendState = gdi.GetBlendState(ss.GetRenderState()->GetBlendState());
		int i = 0;
		for(auto & it : ss.GetRenderResource()->GetSamplerState<VertexShader>())
		{
			mVSSamplerState[i++] = gdi.GetSamplerState(it);
		}
		i = 0;
		for (auto & it : ss.GetRenderResource()->GetSamplerState<PixelShader>())
		{
			mPSSamplerState[i++] = gdi.GetSamplerState(it);
		}
		i = 0;
		for (auto & it : ss.GetRenderResource()->GetSamplerState<GeometryShader>())
		{
			mGSSamplerState[i++] = gdi.GetSamplerState(it);
		}
	}

	//void ComputeGPU::Initialize(ComputeShader * cs, unsigned int buffersize)
	//{
	//	auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

	//	this->cs = cs;
	//	if (!cs->mUAVInfo.empty())
	//	{
	//		for (int i = 0; i < (int)cs->mUAVInfo.size(); i++)
	//		{
	//			UnorderedAccessView uav;
	//			if (cs->mUAVInfo[i].flags == D3D_SIT_UAV_APPEND_STRUCTURED)
	//			{
	//				uav.buffer = CreateBuffer(gdi, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, cs->mUAVInfo[i].size * buffersize, cs->mUAVInfo[i].size, D3D11_USAGE_DEFAULT, (D3D11_CPU_ACCESS_FLAG)0);
	//				uav.uav = CreateUnorderedAccessView(gdi, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, D3D11_BUFFER_UAV_FLAG_APPEND);
	//				uav.srv = CreateShaderResourceView(gdi, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, (D3D11_BUFFEREX_SRV_FLAG)0);
	//				mCSUAV.push_back(uav);
	//			}
	//			if (cs->mUAVInfo[i].flags == D3D_SIT_UAV_CONSUME_STRUCTURED)
	//			{
	//				uav.buffer = CreateBuffer(gdi, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, cs->mUAVInfo[i].size * buffersize, cs->mUAVInfo[i].size, D3D11_USAGE_DEFAULT, (D3D11_CPU_ACCESS_FLAG)0);
	//				uav.uav = CreateUnorderedAccessView(gdi, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, D3D11_BUFFER_UAV_FLAG_APPEND);
	//				uav.srv = CreateShaderResourceView(gdi, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, (D3D11_BUFFEREX_SRV_FLAG)0);
	//				mCSUAV.push_back(uav);
	//			}
	//			if (cs->mUAVInfo[i].flags == D3D_SIT_UAV_RWSTRUCTURED)
	//			{
	//				uav.buffer = CreateBuffer(gdi, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, cs->mUAVInfo[i].size * buffersize, cs->mUAVInfo[i].size, D3D11_USAGE_DEFAULT, (D3D11_CPU_ACCESS_FLAG)0);
	//				uav.uav = CreateUnorderedAccessView(gdi, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, D3D11_BUFFER_UAV_FLAG(0));
	//				uav.srv = CreateShaderResourceView(gdi, uav.buffer, buffersize, DXGI_FORMAT_UNKNOWN, (D3D11_BUFFEREX_SRV_FLAG)0);
	//				mCSUAV.push_back(uav);
	//			}
	//		}
	//	}
	//	InitializeConstantBuffer(cs, mCSCBuffer);
	//	InitializeSampler(cs, mCSSamplerState);
	//	ShaderResourceView srv = { nullptr };
	//	if (cs->GetTextureCount() > 0)
	//		for (unsigned i = 0; i < cs->GetTextureCount(); i++)
	//			mCSSRV.push_back(srv);

	//}
	//void ComputeGPU::Shutdown()
	//{
	//	for (auto it : mCSUAV)
	//	{
	//		it.buffer->Release();
	//		it.srv->Release();
	//		it.uav->Release();
	//	}
	//	ShutdownConstantBuffer(cs, mCSCBuffer);
	//	ShutdownSampler(cs, mCSSamplerState);
	//}
	//void ComputeGPU::SetCSSRV(unsigned int index, ID3D11ShaderResourceView * srv)
	//{
	//	XGF_ASSERT(index < cs->mTexture2D.size());

	//	mCSSRV[index].srv = srv;
	//}
	//void ComputeGPU::SetCSUAV(unsigned int index, ID3D11UnorderedAccessView * uav)
	//{
	//	XGF_ASSERT(index < cs->mUAVInfo.size());
	//	mCSUAV[index].uav = uav;
	//}
	//void ComputeGPU::SetCSSamplerState(unsigned int index, SamplerState state)
	//{
	//	mCSSamplerState[index] = state;
	//}
	//void ComputeGPU::Run(bool asyn)
	//{
	//	auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

	//	gdi.GetDeviceContext()->Dispatch(32, 1, 1);
	//	if (!asyn)//同步等待gpu运行结束
	//	{
	//		D3D11_QUERY_DESC pQueryDesc;
	//		pQueryDesc.Query = D3D11_QUERY_EVENT;
	//		pQueryDesc.MiscFlags = 0;
	//		ID3D11Query *pEventQuery;
	//		gdi.GetDevice()->CreateQuery(&pQueryDesc, &pEventQuery);
	//		gdi.GetDeviceContext()->End(pEventQuery);
	//		while (gdi.GetDeviceContext()->GetData(pEventQuery, NULL, 0, 0) == S_FALSE) {}
	//		pEventQuery->Release();
	//	}
	//}
	//void ComputeGPU::Bind()
	//{
	//	auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

	//	ID3D11Buffer * cbuffer;
	//	for (int i = 0; i < (int)cs->mCBufferInfo.size(); i++)
	//	{
	//		//mCSCBuffer[i].MapToGPU(gdi);
	//		//cbuffer = mCSCBuffer[i].GetRawBuffer();
	//		gdi.GetDeviceContext()->CSSetConstantBuffers(cs->mCBufferInfo[i].slot, 1, &cbuffer);
	//	}

	//	if (!mCSSamplerState.empty())
	//		for (int i = 0; i < (int)cs->GetSamplerStateCount(); i++)
	//		{
	//			auto sps = gdi.GetRawSamplerState(mCSSamplerState[i]);
	//			gdi.GetDeviceContext()->CSSetSamplers(cs->GetSamplerStateSlot(i), 1, &sps);
	//		}
	//	if (!mCSSRV.empty())
	//		for (int i = 0; i < (int)cs->GetTextureCount(); i++)
	//			if (mCSSRV[i].srv != nullptr)
	//				gdi.GetDeviceContext()->CSSetShaderResources(cs->GetTextureSlot(i), 1, &mCSSRV[i].srv);
	//			else { XGF_Warn(Shader, "invalid srv in cs"); }
	//	if (!mCSUAV.empty())
	//		for (int i = 0; i < (int)cs->mUAVInfo.size(); i++)
	//			if (mCSUAV[i].uav != nullptr)
	//				gdi.GetDeviceContext()->CSSetUnorderedAccessViews(cs->mUAVInfo[i].slot, 1, &(mCSUAV[i].uav), 0);
	//			else { XGF_Warn(Shader, "invalid uav in cs"); }

	//	gdi.GetDeviceContext()->CSSetShader(cs->GetShader(), nullptr, 0);


	//}
	//void ComputeGPU::UnBind()
	//{
	//	auto & gdi = Context::Current().QueryGraphicsDeviceInterface();

	//	void* null = 0;
	//	gdi.GetDeviceContext()->CSSetShader((ID3D11ComputeShader *)null, nullptr, 0);
	//	if (!mCSSRV.empty())
	//		for (int i = 0; i < (int)cs->GetTextureCount(); i++)
	//			gdi.GetDeviceContext()->CSSetShaderResources(cs->GetTextureSlot(i), 1, (ID3D11ShaderResourceView **)(&null));
	//	if (!mCSUAV.empty())
	//		for (int i = 0; i < (int)cs->mUAVInfo.size(); i++)
	//			gdi.GetDeviceContext()->CSSetUnorderedAccessViews(cs->mUAVInfo[i].slot, 1, (ID3D11UnorderedAccessView **)(&null), 0);
	//}
	//UnorderedAccessView * ComputeGPU::GetUnorderedAccessViews(int index)
	//{
	//	XGF_ASSERT(index < (int)cs->mUAVInfo.size());
	//	return &mCSUAV[index];
	//}
	//unsigned int ComputeGPU::GetUnorderedAccessViewCount()
	//{
	//	return static_cast<unsigned int>(cs->mUAVInfo.size());
	//}
}
