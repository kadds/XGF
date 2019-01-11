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
		friend class InputManager;
		Cursor();
		~Cursor();
		void Initialize();
		void Shutdown();
		
		void SetStaticTexture(TextureResource * res);
		void SetAnimation(Animation * mm);
		void SetPointDeviation(Position & p) { mPointDeviation = p; };
		
	private:
		void Draw(const WVPMatrix & wvp);
		void SetPosition(float x, float y);
		void Show();
		void Tick(float time);
		void Hide();
		Batch mTextureBatch;
		Position mPosition;
		Position mPointDeviation;
		Position mSize;
		Shape::Rectangle mRc;
		BindingBridge mBbrg;
		float mPassTime;
		bool mIsShow;
		bool mUsedStaticTexture;
		std::shared_ptr<PolygonPleTextureBinder> mPtBinder;

		TextureResource * mTextureResource;
		Texture mTexture;
		Animation *mAnimation;
		DISALLOW_COPY_AND_ASSIGN(Cursor);
	};


}
