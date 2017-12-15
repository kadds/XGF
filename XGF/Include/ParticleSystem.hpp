#pragma once
#include "Batch.hpp"
#include "Shape.hpp"
#include "Rectangle.hpp"
#include "Shader.hpp"
#include <vector>
#include <random>
namespace XGF
{
	//Emitter base
	class Emitter
	{
	public:
		Point & GetPosition() { return pos; };

	protected:

	public:
		Emitter();
		void SetWidthAndHeight(float w, float h) { width = w; height = h; }
		void SetAliveTime(float time);
		void SetGravity(XMFLOAT3 g, float w);
		void SetFrequency(int countPerframe);
		float mAliveTime;
		float mPastTime;
		bool isAlive;
		float width;
		float height;
		Point pos;
		int mMaxParticle;
		//XMFLOAT3 density;//xyz方向上的密度
		int frequency;//频率每秒释放粒子个数
		XMFLOAT3 gravity;//重力 方向 xyz
		XMFLOAT3 direction;
		float startAngleMin;//角度制
		float startAngleMax;
		float endAngleMin;
		float endAngleMax;
		bool mIgnoreZ;

		float delta;
	};

	class ParticleSystem
	{
	public:
		ParticleSystem();
		~ParticleSystem();

	protected:
		Batch mBatch;
		VertexShader mVertexShader;
		GeometryShader mGeometryShader;
		Shaders mShaders;
		PixelShader mPixelShader;
		//ComputeShader mComputerShader;
		bool firstRun;
		bool mUseCPU;//使用CPU计算？？？

		std::random_device rdDevice;
		std::mt19937 mtRandom;
	private:
		
	};

	class ParticleExplosion : public ParticleSystem
	{
	public:
		class ExplosionEmitter : public Emitter
		{
		public:
			ExplosionEmitter();
			float speed;
			float radialAccel;
			float startRadius;
			float endRadius;
			float accelerationMax;
			float accelerationMin;
			float velocityMax;
			float velocityMin;
			Color startColorMin;
			Color startColorMax;
			Color endColorMin;
			Color endColorMax;
			void SetColor(Color & startmin, Color & startmax, Color & endmin, Color & endmax);
			void SetEmitterAngle(float startmin, float startmax, float endmin, float endmax);
			
		};
		struct EmitterInputData
		{
			XMFLOAT3 pos;
			XMFLOAT4 data;
		};
		struct ParticleData
		{
			XMFLOAT3 pos;
			XMFLOAT2 size;
			XMFLOAT3 velocity;//速度 xyz方向
			XMFLOAT3 acceleration;//加速度 xyz方向
			Color color;
			bool used;
			ParticleData(Point p, Position s, Point v, Point acc, Color & c) :pos(p), velocity(v), color(c), acceleration(acc),size(s),used(true){};
		};
		void Draw();
		void Begin(const WVPMatrix & matrix);
		void End();
		void Initialize(GDI * gdi, int count);
		void Shutdown();
		void Reset();
		void AddEmitter(ExplosionEmitter *e) { mEmitter = e; }
		void Updata(float time);
		void SetTexture(Texture *t) { mTexture = t; }

	private:
		GDI * mGDI;
		Texture *mTexture;
		ExplosionEmitter * mEmitter;
		std::vector<ParticleData> mParticles;
		bool mFirst;
	};
}
