#pragma once
#include "Defines.hpp"
#include "Batch.hpp"
#include "Line.hpp"
namespace XGF
{
	class Context;
	class Caret
	{
	public:
		Caret();
		~Caret();

		void Initialize();
		void Shutdown();
		void Draw(const WVPMatrix & wvp);
		void Tick(float time);
		void Show() { mIsCaretShow = true;mTime = 0.f; };
		void Hide() { mIsCaretShow = false; };
		void SetPosition(float x, float y);
		void SetHeight(int height) { mHeight = height; };
		void ResetTime();
	private:
		ShaderStage mShaderStage;
		float mTime;
		//这是间隔标志变量
		bool mHide;
		bool mIsCaretShow;
		Shape::Line mLine;
		BindingBridge mBbrg;
		std::shared_ptr<PolygonPleConstantColorBinder> mPcColor;
		Color mCaretColor;
		int mHeight;

		DISALLOW_COPY_AND_ASSIGN(Caret);
	};


}
