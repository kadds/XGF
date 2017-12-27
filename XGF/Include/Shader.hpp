#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include "gdi.hpp"
#include "Log.hpp"
#include <vector>
#include <string>
namespace XGF
{
	class IConstantBuffer;
	class GDI;
	class ConstantBuffer;
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
	class Shader
	{
	public:
		Shader();
		~Shader();
		void ReflectStage(ID3D11ShaderReflection *reflector);
		unsigned int GetCBufferSize(int index);
		unsigned int GetCBufferCount();
		GDI* GetGDI();
		unsigned int GetCBufferSlot(int index);
		int GetSamplerStateCount();
		unsigned int GetSamplerStateSlot(int index);
		int GetTextureCount();
		unsigned int GetTextureSlot(int index);
	protected:
		friend class ComputeGPU;
		friend class ShaderStage;
		GDI *mGDI;

		std::vector<CBufferInfo> mCBufferInfo;
		std::vector<UAVInfo> mUAVInfo;
		std::vector<TextureInfo> mTexture2D;
		std::vector<SamplerInfo> mSamplerState;

		DISALLOW_COPY_AND_ASSIGN(Shader);
	};
	class PixelShader : public Shader
	{
	public:
		void Initialize(GDI* gdi, const wchar_t* PSname);
		void Initialize(GDI* gdi, const unsigned char* PScode, unsigned int codeLen);
		void Shutdown();
		PixelShader() {};
	private:
		friend class ShaderStage;
		std::vector<ID3D11ShaderResourceView *> mResviews;
		ID3D11PixelShader * mPixelShader;
	};
	class VertexShader : public Shader
	{
	public:
		void Initialize(GDI* gdi, const wchar_t* VSname);
		void Initialize(GDI* gdi, const unsigned char* VScode, unsigned int codeLen);

		void Shutdown();
		void SetInputLayout();
		unsigned int * GetStride();
		unsigned int GetStrideAllSize();
	private:
		friend class ShaderStage;
		ID3D11VertexShader * mVertexShader;
		ID3D11InputLayout * mInputLayout;
		
		unsigned int * mSlotStride;
		int mCount;
	};
	class GeometryShader : public Shader
	{
	public:
		void Initialize(GDI* gdi, const wchar_t* GSname, bool streamOut = false);
		void Initialize(GDI* gdi, const unsigned char* GScode, unsigned int codeLen, bool streamOut = false);
		void Swap();
		void Shutdown();
		bool IsStreamOut() { return mStreamOut; }
		ID3D11Buffer * GetStreamOutBuffer() { return mOutBuffer[0]; };
	private:
		bool mStreamOut;
		friend class ShaderStage;
		ID3D11GeometryShader * mGeometryShader;
		D3D11_PRIMITIVE mPrimitive;
		//2个实现缓冲区交换
		ID3D11Buffer * mOutBuffer[2];
	};
	
	class ComputeShader : public Shader
	{
	public:
		void Initialize(GDI* gdi, const wchar_t* CSname);
		void Initialize(GDI* gdi, const unsigned char* CScode, unsigned int codeLen);
		void Shutdown();
		ID3D11ComputeShader * GetShader() { return mComputeShader; }
		
	private:
		friend class ComputeGPU;
		ID3D11ComputeShader * mComputeShader;
	};


	class ShaderResourceView
	{
	public:
		ID3D11ShaderResourceView * srv;
	};
	class UnorderedAccessView
	{
	public:
		ID3D11UnorderedAccessView * uav;
		ID3D11ShaderResourceView * srv;
		ID3D11Buffer * buffer;
	};
	struct Shaders // 一系列shader
	{
		VertexShader * vs;
		PixelShader * ps;
		GeometryShader * gs;
	};
	//gpu通用计算
	//不会负责建立UAV和SRV
	class ComputeGPU
	{
	private:
		ComputeShader *cs;
		std::vector<SamplerState> mCSSamplerState;
		std::vector<ConstantBuffer> mCSCBuffer;
		std::vector<UnorderedAccessView> mCSUAV;
		std::vector<ShaderResourceView> mCSSRV;
	public:
		void Initialize(ComputeShader * cs, unsigned int buffersize);
		void Shutdown();
		void SetCSSRV(int index, ID3D11ShaderResourceView * srv);
		void SetCSUAV(int index, ID3D11UnorderedAccessView * uav);
		void SetCSSamplerState(int index, SamplerState state);
		void Run(bool asyn);
		void Bind();
		void UnBind();
		UnorderedAccessView * GetUnorderedAccessViews(int index);
		int GetUnorderedAccessViewCount();
	};
	//自动建立buffer
	//可编程阶段
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
		std::vector<ConstantBuffer> mVSCBuffer, mPSCBuffer, mGSCBuffer;
		std::vector<ShaderResourceView> mVSSRV, mPSSRV, mGSSRV;
	public:
		void Initialize(VertexShader * vs, PixelShader * ps = nullptr, GeometryShader * gs = nullptr);
		void Shutdown();
		VertexShader * GetVSShader() { return vs; };
		PixelShader * GetPSShader() { return ps; };
		GeometryShader * GetGSShader() { return gs; };
		void SetBlendState(BlendState  bs);
		void SetDepthStencilState(DepthStencilState ds);
		void SetRasterizerState(RasterizerState rs);
		BlendState GetBlendState();
		void SetVSConstantBuffer(int index, const void * data);
		void SetGSConstantBuffer(int index, const void * data);
		void SetPSConstantBuffer(int index, const void * data);
		template<typename Tshader>
		Shader * GetTemplateShader()
		{
			if (typeid(Tshader) == typeid(VertexShader))
				return vs;
			else if (typeid(Tshader) == typeid(PixelShader))
				return ps;
			else if (typeid(Tshader) == typeid(GeometryShader))
				return gs;
			return nullptr;
		}
		template<typename Tshader>
		int GetConstantBufferIndexByName(const char * name)
		{
			int i = 0;
			Shader * shader = GetTemplateShader<Tshader>();
			
			for (; i < shader->mCBufferInfo.size(); i++)
				if (shader->mCBufferInfo[i].name == name)
					return i;
			if (i == shader->mCBufferInfo.size()) XGF_ReportWarn0("can't find Cbuffer index");
			return 0;
		};
		
		void SetVSSamplerState(int index, SamplerState ss);
		void SetPSSamplerState(int index, SamplerState ss);
		void SetGSSamplerState(int index, SamplerState ss);
		template<typename Tshader>
		int GetSamplerStateIndexByName(const char * name)
		{
			int i;
			Shader * shader = GetTemplateShader<Tshader>();

			for (; i < shader->mSamplerState.size(); i++)
				if (shader->mSamplerState[i].name == name)
					return i;
			if (i == shader->mSamplerState.size()) XGF_ReportWarn0("can't find SamplerState index");
			return 0;
		};

		void SetVSSRV(int index, ID3D11ShaderResourceView * srv );
		void SetGSSRV(int index, ID3D11ShaderResourceView * srv);
		void SetPSSRV(int index, ID3D11ShaderResourceView * srv);
		template<typename Tshader>
		int GetSRVIndexByName(const char * name)
		{
			int i;
			Shader * shader = GetTemplateShader<Tshader>();

			for (; i < shader->mTexture2D.size(); i++)
				if (shader->mTexture2D[i].name == name)
					return i;
			if (i == shader->mTexture2D.size()) XGF_ReportWarn0("can't find srv index");
			return 0;
		}
		
		void BindStage();
		void UnBindStage();
	};
}
