#pragma once
#include "Defines.hpp"
#include "Rectangle.hpp"
#include "Shader.hpp"
#include "Batch.hpp"
class Font;
class Batch;
//typedef std::pair<int, bool> CharState;
//typedef std::list<CharState> CharStateList;

class TextRenderer
{
public:
    TextRenderer();
	~TextRenderer();
	void Initialize(GDI * gdi, Font * font, int MaxCount);
	void Shutdown();
	void DrawString(const wchar_t * str, int x, int y);
    void DrawString(const wchar_t * str, Color color, int x, int y);
    void DrawStringEx(int x, int y, const wchar_t * str, ...);
    void DrawStringEx(int x, int y, Color color, const wchar_t * str, ...);
	Position DrawString(const wchar_t * str, int start, int end, Color color, const Shape::Rectangle * ppe, const XMMATRIX * matrix);
    //void DrawString(const wchar_t * str, int start, int end, Color color, Shape::Rectangle * ppe);
	//void SetColor(DirectX::XMFLOAT4 color) { mBatch->SetColor(color); }
    void Begin(const WVPMatrix & matrix);
    void End();
	void Flush();
private:
	//void Draw(wchar_t c, wchar_t lc, PosSize * ps, int off, Shape::Rectangle & ppe, BindingBridge & bb, DirectX::XMMATRIX * matrix);
	Batch mBatch;
    Shader mShader;
    //Õ‚≤ø
	Font *mFont;
    //std::unordered_map<wchar_t, CharStateList*> map;
    wchar_t * mTemporarybuffer;
};

#define MAX_TEMPORARY_BUFFER_SIZE 512