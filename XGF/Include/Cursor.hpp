#pragma once
#include "Defines.hpp"
#include "Batch.hpp"
#include "Rectangle.hpp"
#include "Texture.hpp"
#include "Animation.hpp"
namespace XGF
{
	class Cursor
	{
	public:
		Cursor();
		~Cursor();
		void Initialize(GDI * gdi);
		void Shutdown();
		void Draw(const WVPMatrix & wvp);
		void SetPosition(float x, float y);
		void Show();
		void Tick(float time);
		void SetStaticTexture(TextureResource * res);
		void SetAnimation(Animation * mm);
		void SetPointDeviation(Position & p) { mPointDeviation = p; };
		void Hide();
	private:
		Batch mTextureBatch;
		Position mPosition;
		Position mPointDeviation;
		Position mSize;
		Shape::Rectangle mRc;
		BindingBridge mBbrg;
		float mPassTime;
		bool mIsShow;
		bool mUsedStaticTexture;
		PolygonPleTextureBinder mPtBinder;

		TextureResource * mTextureResource;
		Texture mTexture;
		Animation *mAnimation;
		DISALLOW_COPY_AND_ASSIGN(Cursor);
	};


}
