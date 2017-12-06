#pragma once
#include "Batch.hpp"
#include "Shape.hpp"
#include "Rectangle.hpp"
#include "Shader.hpp"
namespace XGF
{
	//Emitter base
	class Emitter
	{
	public:
		Point & GetPosition() { return pos; };
	protected:

	public:
		void SetWidthAndHeight(float w, float h) { width = w; height = h; }
		float mAliveTime;
		float mPastTime;
		bool isAlive;
		float width;
		float height;
		Point pos;
	};

	class ParticleSystem
	{
	public:
		ParticleSystem();
		~ParticleSystem();

	protected:
		Batch mBatch;
		Shader mShader;
		Shader mDrawShader;
		Batch mDrawBatch;
		ComputeShader mComputerShader;
		bool firstRun;
		int mMaxcount;
	private:

	};

	class ParticleExplosion : public ParticleSystem
	{
	public:
		class ExplosionEmitter : public Emitter
		{
		public:
			float speed;
			float radialAccel;
			float startRadius;
			float endRadius;
		};
		struct EmitterInputData
		{
			XMFLOAT3 pos;
			XMFLOAT4 data;
		};
		struct EmitterOutPutData
		{
			XMFLOAT3 pos;
			XMFLOAT4 data;
		};
		void Draw();
		void Begin(const WVPMatrix & matrix);
		void End();
		void Initialize(GDI * gdi, int count);
		void Shutdown();
		void AddEmitter(ExplosionEmitter *e) { mEmitters = e; }
	private:
		GDI * mGDI;
		Texture *mTexture;
		ExplosionEmitter * mEmitters;
	};
}
