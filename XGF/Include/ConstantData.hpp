#pragma once
#include "Shader.hpp"
/*
µ¥ÀýÀà
*/
class ConstantData
{
public:
	ConstantData() {};
	~ConstantData() {};
	void Initialize(GDI * gdi);//¶¥²ã´°¿Ú
	void Shutdown();

	Shader * GetFontShader() { return &mFontShader; }
	Shader * GetPTShader() { return &mPTShader; }
	Shader * GetPCShader() { return &mPCShader; }
	Shader * GetPCTShader() { return &mPCTShader; }
	static ConstantData & GetInstance() {
		return mConstantData;
	};
private:
	Shader mFontShader;
	Shader mPTShader;
	Shader mPCShader;
	Shader mPCTShader;
	static ConstantData mConstantData;
};

