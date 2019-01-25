#pragma once
#include "Defines.hpp"
#include "Shader.hpp"
#include "Rectangle.hpp"
#include "Texture.hpp"
#include "Animation.hpp"
#include "Polygon.hpp"

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
		
		void SetStaticTexture(Texture * texture);
		void SetAnimation(Animation * mm);
		void SetPointDeviation(Position & p) { mPointDeviation = p; };
		
	private:
		void Draw(const WVPMatrix & wvp);
		void SetPosition(float x, float y);
		void Show();
		void Tick(float time);
		void Hide();
		ShaderStage mShaderStage;
		Position mPosition;
		Position mPointDeviation;
		Position mSize;
		Shape::Rectangle mRc;
		BindingBridge mBbrg;
		float mPassTime;
		bool mIsShow;
		bool mUsedStaticTexture;
		std::shared_ptr<PolygonPleTextureBinder> mPtBinder;

		Texture * mTexture;
		Animation *mAnimation;
		DISALLOW_COPY_AND_ASSIGN(Cursor);
	};


}
