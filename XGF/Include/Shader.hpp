#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
#include <d3dcompiler.h>
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
		friend class RenderResource;
		std::vector<CBufferInfo> mCBufferInfo;
		std::vector<UAVInfo> mUAVInfo;
		std::vector<TextureInfo> mTexture2D;
		std::vector<SamplerInfo> mSamplerState;

		DisableCopyAndAssign(Shader)
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
		friend class RawRenderStage;
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
		friend class RawRenderStage;
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
		friend class RawRenderStage;
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

	struct ShadersHash
	{
		size_t operator() (const Shaders & shaders) const
		{
			return hash_val(shaders.vs, shaders.ps, shaders.gs);
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

	struct RenderState
	{
		ClassPropertyWithInit(TopologyMode, TopologyMode, TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		ClassProperty(DepthStencilState, DepthStencilState)
		ClassProperty(BlendState, BlendState)
		ClassProperty(RasterizerState, RasterizerState)
	};

	class RenderResource
	{
	private:
		Shaders mShaders;
		std::vector<SamplerState> mVSSamplerState, mPSSamplerState, mGSSamplerState;
		std::vector<StageConstantBuffer> mVSCBuffer, mPSCBuffer, mGSCBuffer;
		std::vector<Texture *> mVSTexture, mPSTexture, mGSTexture;
	public:
		RenderResource(Shaders shaders);
		RenderResource(){};
		void ReCreate(Shaders shaders);
		void Clear();
		~RenderResource();

		bool operator == (const Shaders & shaders) const
		{
			return shaders.vs == mShaders.vs && shaders.ps == mShaders.ps && shaders.gs == mShaders.gs;
		}
		bool operator == (const RenderResource & shaders) const
		{
			return *this == shaders.mShaders;
		}


#pragma region  template of shaders
		template<typename TShader>
		std::vector<StageConstantBuffer> & GetConstantBuffer()
		{
			static_assert(false, "null shader type");
		}
		template<>
		std::vector<StageConstantBuffer> & GetConstantBuffer<VertexShader>()
		{
			return mVSCBuffer;
		}
		template<>
		std::vector<StageConstantBuffer> & GetConstantBuffer<PixelShader>()
		{
			return mPSCBuffer;
		}
		template<>
		std::vector<StageConstantBuffer> & GetConstantBuffer<GeometryShader>()
		{
			return mGSCBuffer;
		}
		template<typename TShader>
		std::vector<Texture *> & GetTexture()
		{
			static_assert(false, "null shader type");
		}
		template<>
		std::vector<Texture *> & GetTexture<VertexShader>()
		{
			return mVSTexture;
		}
		template<>
		std::vector<Texture *> & GetTexture<PixelShader>()
		{
			return mPSTexture;
		}
		template<>
		std::vector<Texture *> & GetTexture<GeometryShader>()
		{
			return mGSTexture;
		}

		template<typename TShader>
		std::vector<SamplerState> & GetSamplerState()
		{
			static_assert(false, "null shader type");
		}
		template<>
		std::vector<SamplerState> & GetSamplerState<VertexShader>()
		{
			return mVSSamplerState;
		}
		template<>
		std::vector<SamplerState> & GetSamplerState<PixelShader>()
		{
			return mPSSamplerState;
		}
		template<>
		std::vector<SamplerState> & GetSamplerState<GeometryShader>()
		{
			return mGSSamplerState;
		}

		template<typename TShader>
		TShader * GetShader()  const
		{
			static_assert(false, "null shader type");
		}
		template<>
		VertexShader * GetShader<VertexShader>() const
		{
			return mShaders.vs;
		}
		template<>
		PixelShader * GetShader<PixelShader>() const
		{
			return mShaders.ps;
		}
		template<>
		GeometryShader * GetShader<GeometryShader>() const
		{
			return mShaders.gs;
		}
#pragma endregion 
		
		template<typename TShader, typename ... Types>
		void SetConstantBuffer(unsigned int index, int offset, Types&&... types)
		{
			constexpr size_t size = sizeof...(types);
			std::pair<const void *, size_t> pack[size];
			Unpack(&pack[0], std::forward<Types>(types)...);
			CopyToConstantBuffer(GetConstantBuffer<TShader>(), GetShader<TShader>(), index, offset, &pack[0], size);
		}
		template<typename First, typename ... Types>
		void Unpack(std::pair<const void *, size_t> * pack, const First & first, Types&& ... types)
		{
			static_assert(!std::is_pointer<First>::value, "Type is pointer, check it");
			*pack = std::make_pair(&first, sizeof(first));
			Unpack(++pack, std::forward<Types>(types)...);
		}
		template<typename First>
		void Unpack(std::pair<const void *, size_t> * pack, const First & first)
		{
			static_assert(!std::is_pointer<First>::value, "Type is pointer, check it");
			*pack = std::make_pair(&first, sizeof(first));
		}

		template<typename TShader>
		void SetConstantBuffer(unsigned int index, void * ptr)
		{
			std::pair<const void *, size_t> pack = std::make_pair(ptr, GetConstantBuffer<TShader>()[index].GetSize());
			CopyToConstantBuffer(GetConstantBuffer<TShader>(), GetShader<TShader>(), index, 0, &pack, 1);
		}

		template<typename TShader, typename ... Types>
		void SetConstantBuffer(unsigned int index, std::function<void(void*, size_t size)> func)
		{
			CopyToConstantBuffer(GetConstantBuffer<TShader>(), GetShader<TShader>(), index, func);
		}

		void CopyToConstantBuffer(std::vector<StageConstantBuffer>& target, Shader* shader, unsigned int index,
		                          int offset, std::pair<const void *, size_t> * ptr, size_t size);

		void CopyToConstantBuffer(std::vector<StageConstantBuffer>& target, Shader* shader, unsigned int index,
		                          std::function<void(void*, size_t size)> func);

		template<typename TShader>
		void SetSamplerState(unsigned int index, SamplerState ss)
		{
			SetSamplerState(GetSamplerState<TShader>(), GetShader<TShader>(), index, ss);
		}

		void SetSamplerState(std::vector<SamplerState>& states, Shader* shader, unsigned int index, SamplerState ss);

		template<typename TShader>
		void SetTexture(unsigned int index, Texture * texture)
		{
			SetTexture(GetTexture<TShader>(), GetShader<TShader>(), index, texture);
		}

		void SetTexture(std::vector<Texture *>& textures, Shader * shader, unsigned index, Texture* texture);

	};

	class RenderStage
	{
		ClassProperty(RenderState, RenderState*)
		ClassProperty(RenderResource, RenderResource*)
	public:
		RenderStage(RenderState * state, RenderResource * resource): mRenderState(state), mRenderResource(resource) {  }
		RenderStage(RenderState & state, RenderResource * resource) : mRenderState(&state), mRenderResource(resource) {  }
		RenderStage(RenderState * state, RenderResource & resource) : mRenderState(state), mRenderResource(&resource) {  }
		RenderStage(RenderState & state, RenderResource & resource) : mRenderState(&state), mRenderResource(&resource) {  }
	};

	class RawRenderStage
	{
	public:
		VertexShader * vs;
		PixelShader * ps;
		GeometryShader * gs;
		RawRasterizerState mRasterizerState;
		RawBlendState mBlendState;
		RawDepthStencilState mDepthStencilState;
		std::vector<RawSamplerState> mVSSamplerState, mPSSamplerState, mGSSamplerState;
		std::vector<StageConstantBuffer> mVSCBuffer, mPSCBuffer, mGSCBuffer;
		std::vector<Texture *> mVSTexture, mPSTexture, mGSTexture;
		TopologyMode mTopologyMode;
		RawRenderStage(const RenderStage& ss);

		void BindStage();
		void UnBindStage();

#pragma region  template of shaders
		template<typename TShader>
		std::vector<StageConstantBuffer> & GetConstantBuffer()
		{
			static_assert(false, "null shader type");
		}
		template<>
		std::vector<StageConstantBuffer> & GetConstantBuffer<VertexShader>()
		{
			return mVSCBuffer;
		}
		template<>
		std::vector<StageConstantBuffer> & GetConstantBuffer<PixelShader>()
		{
			return mPSCBuffer;
		}
		template<>
		std::vector<StageConstantBuffer> & GetConstantBuffer<GeometryShader>()
		{
			return mGSCBuffer;
		}
		template<typename TShader>
		std::vector<Texture *> & GetTexture()
		{
			static_assert(false, "null shader type");
		}
		template<>
		std::vector<Texture *> & GetTexture<VertexShader>()
		{
			return mVSTexture;
		}
		template<>
		std::vector<Texture *> & GetTexture<PixelShader>()
		{
			return mPSTexture;
		}
		template<>
		std::vector<Texture *> & GetTexture<GeometryShader>()
		{
			return mGSTexture;
		}

		template<typename TShader>
		std::vector<RawSamplerState> & GetSamplerState()
		{
			static_assert(false, "null shader type");
		}
		template<>
		std::vector<RawSamplerState> & GetSamplerState<VertexShader>()
		{
			return mVSSamplerState;
		}
		template<>
		std::vector<RawSamplerState> & GetSamplerState<PixelShader>()
		{
			return mPSSamplerState;
		}
		template<>
		std::vector<RawSamplerState> & GetSamplerState<GeometryShader>()
		{
			return mGSSamplerState;
		}

		template<typename TShader>
		TShader * GetShader()  const
		{
			static_assert(false, "null shader type");
		}
		template<>
		VertexShader * GetShader<VertexShader>() const
		{
			return vs;
		}
		template<>
		PixelShader * GetShader<PixelShader>() const
		{
			return ps;
		}
		template<>
		GeometryShader * GetShader<GeometryShader>() const
		{
			return gs;
		}
#pragma endregion 

	};
}

