#pragma once
#include "Shader.hpp"
/*
������
*/
class ConstantData
{
public:
	ConstantData() {};
	~ConstantData() {};
	void Initialize(GDI * gdi);//���㴰��
	void Shutdown();

	Shader * GetFontShader() { return &mFontShader; }
	Shader * GetPTShader() { return &mPTShader; }
	Shader * GetPCShader() { return &mPCShader; }
	static ConstantData & GetInstance() {
		return mConstantData;
	};
private:
	Shader mFontShader;
	Shader mPTShader;
	Shader mPCShader;
	static ConstantData mConstantData;
};

