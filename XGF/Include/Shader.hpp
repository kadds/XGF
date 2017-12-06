#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>
namespace XGF
{
	class IConstantBuffer;
	class GDI;
	class EnumLayout
	{
	public:
		char *name;
		unsigned int size;
		unsigned char number;
		EnumLayout(char * n, unsigned int l, unsigned char num) :name(n), size(l), number(num) {};
	};
	class ShaderLayout
	{
	public:
		const EnumLayout * enumLayout;
		bool mConnet;
		bool mInstance;
		ShaderLayout(const EnumLayout * el, bool cn = false, bool instance = false) :enumLayout(el), mConnet(cn), mInstance(instance) {};
	};
	struct SlotCan
	{
		int size;
		bool nextIsASlot;
	};

	class Shader
	{
	public:
		Shader();
		~Shader();
		virtual void SetShaderConstantParameter(IConstantBuffer * cb) {};

	protected:
		GDI *mGDI;
		DISALLOW_COPY_AND_ASSIGN(Shader);
	};
	class PixelShader : public Shader
	{
	public:
		void Initialize(GDI* gdi, const wchar_t* PSname);
		void Initialize(GDI* gdi, const unsigned char* PScode, unsigned int codeLen);
		void Shutdown();
		virtual void SetShaderConstantParameter(IConstantBuffer * cb) override;
	private:
		friend class Shaders;
		ID3D11PixelShader *mPixelShader;
	};
	class VertexShader : public Shader
	{
	public:
		void Initialize(GDI* gdi, const wchar_t* VSname, ShaderLayout layout[], int layoutCount);
		void Initialize(GDI* gdi, const unsigned char* VScode, unsigned int codeLen, ShaderLayout layout[], int layoutCount);

		void Shutdown();
		void SetInputLayout();
		int GetSlotCount();
		virtual void SetShaderConstantParameter(IConstantBuffer * cb) override;
		void GetStride(unsigned int stride[]);
		int GetSize(int position);
		SlotCan * GetSlotCan() { return mSlotStride; }
	private:
		friend class Shaders;
		ID3D11VertexShader *mVertexShader;
		ID3D11InputLayout *mInputLayout;
		SlotCan * mSlotStride;
		int mCount;
	};
	class GeometryShader : public Shader
	{
	public:
		void Initialize(GDI* gdi, const wchar_t* GSname);
		void Shutdown();
		virtual void SetShaderConstantParameter(IConstantBuffer * cb) override;
	private:
		friend class Shaders;
		ID3D11GeometryShader * mGeometryShader;
	};



	extern const EnumLayout SHADER_EL_POSITION4;
	extern const EnumLayout SHADER_EL_POSITION3;
	extern const EnumLayout SHADER_EL_COLOR;
	extern const EnumLayout SHADER_EL_TEXTURE;
	extern const EnumLayout SHADER_EL_NORMAL;
	extern const EnumLayout SHADER_EL_SV_POSITION;
	enum class ComputeLayout
	{
		Float4,
		Float3,
		Float2,
	};
	class ComputeShader : Shader
	{
	public:
		void Initialize(GDI* gdi, const wchar_t* CSname, int maxcount, int constantByteWidth, int inputByteWidth, int outputByteWidth);
		void Shutdown();
		void Run();
		ID3D11Buffer * GetOutBuffer() {
			return mOutputBuffer[1];
		};
		ID3D11ShaderResourceView * GetSRV() { return mInputShaderResourceView[1]; }
		ID3D11UnorderedAccessView * GetUAV() { return mUnorderedAccessView[1]; };
		void* Begin();
		void End();
	private:
		ID3D11ComputeShader * mComputeShader;
		ID3D11Buffer* mCBMatrixBuffer; //³£Á¿»º´æ
		ID3D11Buffer * mOutputBuffer[2];
		ID3D11ShaderResourceView *mInputShaderResourceView[2];
		ID3D11UnorderedAccessView *mUnorderedAccessView[2];
	};
	class Shaders
	{
	private:
		VertexShader * vs;
		PixelShader * ps;
		GeometryShader * gs;
	public:
		void Initialize(VertexShader * vs, PixelShader * ps = nullptr, GeometryShader * gs = nullptr)
		{
			this->vs = vs;
			this->gs = gs;
			this->ps = ps;
		}
		VertexShader * GetVSShader() { return vs; };
		PixelShader * GetPSShader() { return ps; };
		GeometryShader * GetGSShader() { return gs; };
		void SetVSConstantBuffer(IConstantBuffer * ib) { vs->SetShaderConstantParameter(ib); };
		void SetPSConstantBuffer(IConstantBuffer * ib) { ps->SetShaderConstantParameter(ib); };
		void SetGSConstantBuffer(IConstantBuffer * ib) { gs->SetShaderConstantParameter(ib); };
		void BindShader();
		void UnBindShader();
	};
}
