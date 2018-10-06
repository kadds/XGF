#pragma once
#include "Defines.hpp"
#include "Texture.hpp"
#include "Rectangle.hpp"
using std::vector;
namespace XGF
{
	struct AnimationStage
	{
		float left;
		float top;
		float right;
		float bottom;
		AnimationStage(float leftx, float topx, float rightx, float bottomx) :left(leftx), top(topx), right(rightx), bottom(bottomx) {};
		AnimationStage() {};
	};

	class Animation
	{
	public:
		Animation(int ID) :passTime(0.f), mIsStop(true), mStages(nullptr) { mID = ID; }
		~Animation();
		bool IsStop() { return mIsStop; }
		void Stop() { mIsStop = true; }
		void Tick(float time);
		void SetAnimation(int LoopTimes/*0 :loop forever, 1 once, -1 never*/, float frameTime, float delayTime, Texture & t, AnimationStage stages[], int stageCount);
		void GetPositionInTexture(float & left, float & top, float & right, float & bottom);
		Texture * GetTexture();
	private:
		float passTime;
		bool mIsStop;
		int mID;
		int mLoopTimes;
		float mDelayTime;
		float mFrameTime;
		Texture mTexture;
		AnimationStage *mStages;
		int mCount;
		int mNowCount;
	};


}
