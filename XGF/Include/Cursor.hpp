#pragma once
#include "Defines.hpp"
#include "Batch.hpp"
#include "Rectangle.hpp"
#include "Texture.hpp"
class Cursor
{
public:
	Cursor();
	~Cursor();
	void Initialize(GDI * gdi);
	void Shutdown();
	void Draw(const WVPMatrix & wvp);
	void SetPosition(int x, int y);
private:
	Batch mTextureBatch;
	int mx;
	int my;
	Shape::Rectangle mRc;
	BindingBridge mBbrg;
	bool mIsShow;
	PolygonPleTextureBinder mPtBinder;

	Texture mTexture;
	DISALLOW_COPY_AND_ASSIGN(Cursor);
};

