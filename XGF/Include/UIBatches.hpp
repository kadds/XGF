#pragma once
#include "Defines.hpp"
#include "Batches.hpp"
#include "Batch.hpp"
#include "OrthoCamera.hpp"
#include "Shader.hpp"
/*
用于UI系统（不包括字体渲染）的Batch集合
*/
class UIBatches
{
public:
	void Initialize(GDI * gdi, Batches & batches, unsigned int CmaxVetices = 1024, unsigned int CmaxIndices = 1024 * 2, unsigned int TmaxVetices = 1024, unsigned int TmaxIndices = 1024 *2);
	void Shutdown(Batches & batches);

	void Begin();
	void End();
	void OnSize(int x, int y);
	UIBatches();
	~UIBatches();
private:
	OrthoCamera mCamera;
	Batch mBatchPC;
	Batch mBatchPT;
	Shader mShaderPT;
	Shader mShaderPC;
	WVPMatrix * wvp;
};

