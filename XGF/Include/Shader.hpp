#pragma once
#include "Defines.hpp"
#include <d3d11_1.h>

typedef short int InputType;
class GDI;
class Shader
{
public:
	Shader();
	~Shader();
	void Initialize(GDI* gdi, const wchar_t* VSname, const wchar_t* PSname, InputType it[], int len);
	void Initialize(GDI* gdi, const unsigned char *constVSShader, unsigned int VSSize, const unsigned char *constPSShader, unsigned int PSSize, InputType it[], int len);
	//virtual void CreateInputLayout(GDI* gdi) = 0;
	void SetShaderParameter(const WVPMatrix & Matrix);

	void Shutdown();
    int GetSlotCount() { return mCount; }
    unsigned * GetSizePreSlot() { return sizePos; }
    unsigned GetStrides() { return strides; }
    unsigned GetSizeInSlot(int i) { return sizePos[i]; }
    void SetShaderAndInputLayout();
protected:
	ID3D11PixelShader *mPixelShader;
	ID3D11VertexShader *mVertexShader;
	
	ID3D11InputLayout *mInputLayout;
	ID3D11Buffer* mCBMatrixBuffer; //(常量)缓存,顶点索引也是用这个类型
    GDI *mGDI;
    unsigned *sizePos;
    unsigned strides;
    int mCount;
	void InitializeOther(InputType it[], int len,const void * VSpointer, unsigned int VSsize);
	DISALLOW_COPY_AND_ASSIGN(Shader);
};
#define SHADER_INPUTLAYOUT_POSITION 1
#define SHADER_INPUTLAYOUT_COLOR 11
#define SHADER_INPUTLAYOUT_TEXTURE 21