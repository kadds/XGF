#pragma once
#include "Defines.hpp"
#include "Batches.hpp"
#include "Batch.hpp"
#include "OrthoCamera.hpp"
#include "Shader.hpp"
#include "TextRenderer.hpp"
#include <unordered_map>

extern const int BATCHES_TEXTRENDERER_DEFAULT_SIZE;
extern const int BATCHES_TEXTRENDERER_SMALL_SIZE;
extern const int BATCHES_TEXTRENDERER_LARGE_SIZE;
extern const int BATCHES_TEXTRENDERER_LARGEER_SIZE;
extern const int BATCHES_TEXTRENDERER_HUGE_SIZE;
extern const int BATCHES_TEXTRENDERER_HUGEER_SIZE;
extern const int BATCHES_TEXTRENDERER_SMALLER_SIZE;
extern const int BATCHES_TEXTRENDERER_TINY_SIZE;

/*
用于UI系统（包括字体渲染）的Batch集合
*/
class UIBatches : Batches
{
public:
	UIBatches();
	virtual ~UIBatches();
	virtual void Begin(WVPMatrix & wvp) override;
	virtual void End() override;
	TextRenderer * GetTextRenderer(int size) { auto p = maps.find(size);if (p != maps.end()) return p->second; return nullptr; };
	void SetTextRenderer(int size, TextRenderer * t) { maps[size] = t; }
private:
	std::unordered_map<int, TextRenderer * > maps;
};

