#pragma once
#include "Defines.hpp"
#include "Rectangle.hpp"
#include "Shader.hpp"
#include "Batch.hpp"
#include <sstream>
class Font;
class Batch;
/*
通用文本渲染
*/
class TextRenderer
{
public:
    TextRenderer();
	~TextRenderer();
	void Initialize(GDI * gdi, Font * font, int MaxCount);
	void Shutdown();
	void DrawString(const wchar_t * str, float x, float y);
    void DrawString(const wchar_t * str, Color color, float x, float y);
	//低性能，不推荐
	template <typename T>
	void DrawStringWithNum(const wchar_t * str, T c, float x, float y)
	{
		std::wstringstream  strk;
		strk.str(str);
		strk << c;
		DrawString(strk.str().c_str(), Color(1.0f, 1.0f, 1.0f, 1.0f), x, y);
	}
	//低性能，不推荐
    void DrawStringEx(float x, float y, const wchar_t * str, ...);
	//低性能，不推荐
    void DrawStringEx(float x, float y, Color color, const wchar_t * str, ...);
	Position DrawString(const wchar_t * str, int start, int end, Color color, const Shape::Rectangle * ppe, const XMMATRIX * matrix);

    void Begin(const WVPMatrix & matrix);
    void End();
	void Flush();
private:
	Batch mBatch;
    Shader mShader;
    //外部
	Font *mFont;
    //std::unordered_map<wchar_t, CharStateList*> map;
    wchar_t * mTemporarybuffer;
};

#define MAX_TEMPORARY_BUFFER_SIZE 512