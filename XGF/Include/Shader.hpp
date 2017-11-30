#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>

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
	ShaderLayout(const EnumLayout * el, bool cn = false) :enumLayout(el),mConnet(cn){};
};
struct SlotCan
{
	union
	{
		UINT * address;
		UINT data;
	} size;//count = 1, using the data else using the address for array
	
	int count;
};
class Shader
{
public:
	Shader();
	~Shader();
	void Initialize(GDI* gdi, const wchar_t* VSname, const wchar_t* PSname, ShaderLayout it[], int len);
	void Initialize(GDI* gdi, const wchar_t* VSname, const wchar_t* PSname, const wchar_t* Geometryname, ShaderLayout it[], int len, EnumLayout ot[], int olen);

	void Initialize(GDI* gdi, const unsigned char *constVSShader, unsigned int VSSize, const unsigned char *constPSShader, unsigned int PSSize, ShaderLayout it[], int len);
	//virtual void CreateInputLayout(GDI* gdi) = 0;
	void SetShaderParameter(const WVPMatrix & Matrix);

	void Shutdown();
    int GetSlotCount() { return mCount; }
	SlotCan * GetSizePreSlot() { return sizePos; }
    UINT * GetSizeInSlot(int i) { return sizePos[i].count == 1 ? &sizePos[i].size.data : sizePos[i].size.address; }
	UINT GetAllSizeInOneSlot(int i);
	void GetAllSizeInSlot(UINT dout[]);
	int GetCountInSlot(int i) { return sizePos[i].count; }
    void SetShaderAndInputLayout();

protected:
	ID3D11PixelShader *mPixelShader;
	ID3D11VertexShader *mVertexShader;
	ID3D11GeometryShader * mGeometryShader;

	ID3D11InputLayout *mInputLayout;
	ID3D11Buffer* mCBMatrixBuffer; //³£Á¿»º´æ
    GDI *mGDI;
    SlotCan *sizePos;
    int mCount;
	void InitializeOther(ShaderLayout it[], int len,const void * VSpointer, unsigned int VSsize);
	DISALLOW_COPY_AND_ASSIGN(Shader);
};


extern const EnumLayout SHADER_EL_POSITION4;
extern const EnumLayout SHADER_EL_POSITION3;
extern const EnumLayout SHADER_EL_COLOR;
extern const EnumLayout SHADER_EL_TEXTURE;
extern const EnumLayout SHADER_EL_NORMAL;
extern const EnumLayout SHADER_EL_SV_POSITION;