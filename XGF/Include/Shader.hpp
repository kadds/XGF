#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include "gdi.hpp"
namespace XGF
{
	class IConstantBuffer;
	class GDI;
	class ConstantBuffer;
	struct CBufferInfo
	{
		const char * name;
		unsigned int slot;
		unsigned int size;
	};
	struct TextureInfo
	{
		const char * name;
		unsigned int slot;
	};
	struct SamplerInfo
	{
		const char * name;
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
		GDI *mGDI;
		CBufferInfo * mCBufferInfo;

		TextureInfo * mTexture2D;//SRV in slot
		SamplerInfo * mSamplerState;//ss in slot
		int mCBufferCount;
		int mBindTextureCount;
		int mBindSamplerCount;
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
		ID3D11ShaderResourceView **mResviews;
		ID3D11PixelShader *mPixelShader;
	};
	class VertexShader : public Shader
	{
	public:
		//多slot 输入ShaderSlot 位模式 置 0 1
		void Initialize(GDI* gdi, const wchar_t* VSname);
		void Initialize(GDI* gdi, const unsigned char* VScode, unsigned int codeLen);

		void Shutdown();
		void SetInputLayout();
		unsigned int * GetStride();
		unsigned int GetStrideAllSize();
	private:
		friend class ShaderStage;
		ID3D11VertexShader *mVertexShader;
		ID3D11InputLayout *mInputLayout;
		
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
		ID3D11Buffer * mOutBuffer[2];
	};


	class ComputeShader : Shader
	{
	public:
		void Initialize(GDI* gdi, const wchar_t* CSname);
		void Initialize(GDI* gdi, const unsigned char* CScode, unsigned int codeLen);
		void Shutdown();
		void Run();
		
	private:
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
	};
	struct Shaders
	{
		VertexShader * vs;
		PixelShader * ps;
		GeometryShader * gs;
	};
	class ComputeGPU
	{
	private:
		ComputeShader *cs;
		SamplerState * mCSSamplerState;
		ConstantBuffer * mCSCBuffer;
		UnorderedAccessView *mCSUAV;
		ShaderResourceView *mCSSRV;
	public:
		void Initialize(ComputeShader * cs);
		void Shutdown();
		void SetCSSRV(int index, ID3D11ShaderResourceView * srv);
		void SetCSUAV(int index, ID3D11UnorderedAccessView * uav);
	};
	class ShaderStage
	{
	private:
		VertexShader * vs;
		PixelShader * ps;
		GeometryShader * gs;
		BlendState mBlendState;
		DepthStencilState mDepthStencilState;
		SamplerState *mVSSamplerState, *mPSSamplerState, *mGSSamplerState;
		ConstantBuffer * mVSCBuffer, *mPSCBuffer,* mGSCBuffer;
		ShaderResourceView * mVSSRV, * mPSSRV, * mGSSRV;
		UnorderedAccessView * mPSUAV;
	public:
		void Initialize(VertexShader * vs, PixelShader * ps = nullptr, GeometryShader * gs = nullptr);
		void Shutdown();
		VertexShader * GetVSShader() { return vs; };
		PixelShader * GetPSShader() { return ps; };
		GeometryShader * GetGSShader() { return gs; };
		void SetBlendState(BlendState  bs);
		void SetDepthStencilState(DepthStencilState ds);
		BlendState GetBlendState();
		void SetVSConstantBuffer(int index, const void * data);
		void SetGSConstantBuffer(int index, const void * data);
		void SetPSConstantBuffer(int index, const void * data);

		void SetVSSamplerState(int index, SamplerState ss);
		void SetPSSamplerState(int index, SamplerState ss);
		void SetGSSamplerState(int index, SamplerState ss);

		void SetVSSRV(int index, ID3D11ShaderResourceView * srv );
		void SetGSSRV(int index, ID3D11ShaderResourceView * srv);
		void SetPSSRV(int index, ID3D11ShaderResourceView * srv);

		
		void BindStage();
		void UnBindStage();
	};
}
