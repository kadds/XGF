#pragma once
#include "Defines.hpp"
#include "Batches.hpp"
#include "Batch.hpp"
#include "OrthoCamera.hpp"
#include "Shader.hpp"
#include "TextRenderer.hpp"
#include <unordered_map>
namespace XGF
{
	enum class FontSize {
		Default,
		Small,
		Tiny,
		Large,
		Huge,
	};
	/*
	用于UI系统（包括字体渲染）的Batch集合
	*/
	class UIBatches : public Batches
	{
	public:
		UIBatches();
		virtual ~UIBatches();
		virtual void Begin(WVPMatrix & wvp) override;
		virtual void End() override;
		TextRenderer * GetTextRenderer(FontSize size);
		void SetTextRenderer(FontSize size, TextRenderer * t) { maps[size] = t; }

	private:
		std::unordered_map<FontSize, TextRenderer * > maps;

	};


}
