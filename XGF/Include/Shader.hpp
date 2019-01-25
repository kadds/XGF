#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include "Logger.hpp"
#include <functional>
#include <string>
#include "GDI.hpp"

namespace XGF
{
	class Texture;

	struct CBufferInfo
	{
		std::string name;
		unsigned int slot;
		unsigned int size;
	};
	struct TextureInfo
	{
		std::string name;
		unsigned int slot;
	};
	struct UAVInfo
	{
		std::string name;
		unsigned int slot;
		D3D_SHADER_INPUT_TYPE flags;
		unsigned int size;
	};
	struct SamplerInfo
	{
		std::string name;
		unsigned int slot;
	};
	typedef D3D_PRIMITIVE_TOPOLOGY TopologyMode;

	class Shader
	{
	public:
		Shader();
		virtual ~Shader();
		virtual void Shutdown() = 0;
		void ReflectStage(ID3D11ShaderReflection *reflector);
		unsigned int GetCBufferSize(unsigned int index);
		unsigned int GetCBufferCount();
		unsigned int GetCBufferSlot(unsigned int index);
		unsigned int GetSamplerStateCount();
		unsigned int GetSamplerStateSlot(unsigned int index);
		unsigned int GetTextureCount();
		unsigned int GetTextureSlot(unsigned int index);
	protected:
		friend class ComputeGPU;
		friend class ShaderStage;

		std::vector<CBufferInfo> mCBufferInfo;
		std::vector<UAVInfo> mUAVInfo;
		std::vector<TextureInfo> mTexture2D;
		std::vector<SamplerInfo> mSamplerState;

		DISALLOW_COPY_AND_ASSIGN(Shader);
	};
	class PixelShader : public Shader
	{
	public:
		void Initialize(const unsigned char* PScode, unsigned int codeLen);
		void Shutdown() override;
		PixelShader() {};
		static const std::string & GetEntrypoint();
		static const std::string & GetPrefixName();
	private:
		friend class ShaderStage;
		ID3D11PixelShader * mPixelShader;
		static const std::string mEntrypoint;
		static const std::string mPerfixName;
	};
	class VertexShader : public Shader
	{
	public:
		//interval = 0 
		void Initialize(const unsigned char* VScode, unsigned int codeLen, unsigned int interval = 0);

		void Shutdown() override;
		const unsigned int * GetStrideAtSlot(unsigned int slot) const;
		const unsigned int * GetSlotElementStartPositionArray() const;
		unsigned int GetStrideAllSizeAtSlot(unsigned int slot) const;
		unsigned int GetSlotCount() const;
		unsigned int GetInputCount() const;
		const unsigned int * GetStride() const;
		unsigned int GetInputLayoutCount() const;
		bool HasSemanticTexcoord();
		bool HasSemanticNormal();
		bool HasSemantic(const char * name);
		static const std::string & GetEntrypoint();
		static const std::string & GetPrefixName();
	private:
		friend class ShaderStage;
		static const std::string mEntrypoint;
		static const std::string mPerfixName;
		ID3D11VertexShader * mVertexShader;
		ID3D11InputLayout * mInputLayout;
		class InputElement
		{
		public:
			std::string name;
			int index;
			int slot;
			unsigned stride;
			InputElement(const std::string & name, int index, int slot, unsigned stride);
		};
		std::vector<InputElement> mInputElements;

		std::vector<unsigned int> mSlotStride;
		std::vector<unsigned int> mSlotElementStartPosition;
		unsigned int mLayoutCount;
	};
	class GeometryShader : public Shader
	{
	public:
		void Initialize(const unsigned char* GScode, unsigned int codeLen, bool streamOut = false);
		void Swap();
		void Shutdown() override;
		bool IsStreamOut() { return mStreamOut; }
		ID3D11Buffer * GetStreamOutBuffer() { return mOutBuffer[0]; };
		static const std::string & GetEntrypoint();
		static const std::string & GetPrefixName();
	private:
		static const std::string mEntrypoint;
		static const std::string mPerfixName;
		bool mStreamOut;
		friend class ShaderStage;
		ID3D11GeometryShader * mGeometryShader;
		D3D11_PRIMITIVE mPrimitive;

		ID3D11Buffer * mOutBuffer[2];
	};
	
	class ComputeShader : public Shader
	{
	public:
		void Initialize(const unsigned char* CScode, unsigned int codeLen);
		void Shutdown() override;
		ID3D11ComputeShader * GetShader() { return mComputeShader; }
		static const std::string & GetEntrypoint();
		static const std::string & GetPrefixName();
	private:
		friend class ComputeGPU;
		static const std::string mEntrypoint;
		static const std::string mPerfixName;
		ID3D11ComputeShader * mComputeShader;
	};

	class ShaderResourceView
	{
	public:
		ID3D11ShaderResourceView * srv;
		ShaderResourceView(ID3D11ShaderResourceView * srv): srv(srv) {  }
	};
	class UnorderedAccessView
	{
	public:
		ID3D11UnorderedAccessView * uav;
		ID3D11ShaderResourceView * srv;
		ID3D11Buffer * buffer;
	};
	struct Shaders
	{
		VertexShader * vs;
		PixelShader * ps;
		GeometryShader * gs;
	public:
		Shaders(VertexShader * vs, PixelShader * ps, GeometryShader * gs = nullptr) : vs(vs), ps(ps), gs(gs) {  }
		Shaders() : vs(nullptr), ps(nullptr), gs(nullptr) {  }

		bool operator == (const Shaders & shaders) const
		{
			return shaders.vs == vs && shaders.ps == ps && shaders.gs == gs;
		}
		bool operator != (const Shaders & shaders) const
		{
			return shaders.vs != vs || shaders.ps != ps || shaders.gs != gs;
		}
		bool IsNullShaders() const
		{
			return vs == nullptr && gs == nullptr && ps == nullptr;
		}
	};
	class  StageConstantBuffer
	{
	public:
		std::shared_ptr<char > ptr;
		size_t size;
		StageConstantBuffer(unsigned size);
		StageConstantBuffer(const StageConstantBuffer& scb);
		void* GetBufferPoint() const;

		size_t GetSize() const;
	};
	/*class ComputeGPU
	{
	private:
		ComputeShader *cs;
		std::vector<SamplerState> mCSSamplerState;
		std::vector<StageConstantBuffer> mCSCBuffer;
		std::vector<UnorderedAccessView> mCSUAV;
		std::vector<ShaderResourceView> mCSSRV;
	public:
		void Initialize(ComputeShader * cs, unsigned int buffersize);
		void Shutdown();
		void SetCSSRV(unsigned int index, ID3D11ShaderResourceView * srv);
		void SetCSUAV(unsigned int index, ID3D11UnorderedAccessView * uav);
		void SetCSSamplerState(unsigned int index, SamplerState state);
		void Run(bool asyn);
		void Bind();
		void UnBind();
		UnorderedAccessView * GetUnorderedAccessViews(int index);
		unsigned int GetUnorderedAccessViewCount();
	};
*/
	
	class ShaderStage
	{
	private:
		VertexShader * vs;
		PixelShader * ps;
		GeometryShader * gs;
		RasterizerState mRasterizerState;
		BlendState mBlendState;
		DepthStencilState mDepthStencilState;
		std::vector<SamplerState> mVSSamplerState, mPSSamplerState, mGSSamplerState;
		std::vector<StageConstantBuffer> mVSCBuffer, mPSCBuffer, mGSCBuffer;
		std::vector<Texture *> mVSTexture, mPSTexture, mGSTexture;
		TopologyMode mTopologyMode;
	public:
		bool EqualsWithShaders(const Shaders & shaders);
		void Initialize(VertexShader * vs, PixelShader * ps = nullptr, GeometryShader * gs = nullptr);
		void Initialize(Shaders shaders);
		void Shutdown();
		void SetTopologyMode(TopologyMode topologyMode);

		VertexShader * GetVSShader() { return vs; };
		PixelShader * GetPSShader() { return ps; };
		GeometryShader * GetGSShader() { return gs; };

		const VertexShader * GetVSShader() const { return vs; };
		const PixelShader * GetPSShader() const { return ps; };
		const GeometryShader * GetGSShader() const { return gs; };
		void SetBlendState(BlendState  bs);
		void SetDepthStencilState(DepthStencilState ds);
		void SetRasterizerState(RasterizerState rs);
		BlendState GetBlendState();
		std::vector<StageConstantBuffer> & GetVSConstantBuffer()
		{
			return mVSCBuffer;
		}
		std::vector<StageConstantBuffer> & GetPSConstantBuffer()
		{
			return mPSCBuffer;
		}
		std::vector<StageConstantBuffer> & GetGSConstantBuffer()
		{
			return mGSCBuffer;
		}
		void SetVSConstantBuffer(unsigned int index, const void * data);
		void SetGSConstantBuffer(unsigned int index, const void * data);
		void SetPSConstantBuffer(unsigned int index, const void * data);
		void SetPSConstantBuffer(unsigned index, std::function<void(void*, int size)> func);
		template<typename Tshader>
		Shader * GetTemplateShader()
		{
			return nullptr;
		}
		template<>
		Shader * GetTemplateShader<VertexShader>()
		{
			return vs;
		}
		template<>
		Shader * GetTemplateShader<PixelShader>()
		{
			return ps;
		}
		template<>
		Shader * GetTemplateShader<GeometryShader>()
		{
			return gs;
		}


		template<typename Tshader>
		int GetConstantBufferIndexByName(const char * name)
		{
			unsigned int i = 0;
			Shader * shader = GetTemplateShader<Tshader>();
			
			for (; i < shader->mCBufferInfo.size(); i++)
				if (shader->mCBufferInfo[i].name == name)
					return i;
			if (i == shader->mCBufferInfo.size()) XGF_Warn(Render, "can't find Cbuffer index");
			return 0;
		};
		
		void SetVSSamplerState(unsigned int index, SamplerState ss);
		void SetPSSamplerState(unsigned int index, SamplerState ss);
		void SetGSSamplerState(unsigned int index, SamplerState ss);
		template<typename Tshader>
		int GetSamplerStateIndexByName(const char * name)
		{
			int i;
			Shader * shader = GetTemplateShader<Tshader>();

			for (; i < shader->mSamplerState.size(); i++)
				if (shader->mSamplerState[i].name == name)
					return i;
			if (i == shader->mSamplerState.size()) XGF_Warn(Render, "can't find SamplerState index");
			return 0;
		};

		void SetVSTexture(unsigned int index, Texture * texture);
		void SetGSTexture(unsigned int index, Texture * texture);
		void SetPSTexture(unsigned int index, Texture * texture);
		template<typename Tshader>
		int GetSRVIndexByName(const char * name)
		{
			int i;
			Shader * shader = GetTemplateShader<Tshader>();

			for (; i < shader->mTexture2D.size(); i++)
				if (shader->mTexture2D[i].name == name)
					return i;
			if (i == shader->mTexture2D.size()) XGF_Warn(Render, "can't find srv index");
			return 0;
		}
		
		void BindStage();
		void UnBindStage();

	};
}

