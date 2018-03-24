#pragma once
#include "Defines.hpp"
#include "Batch.hpp"
#include "Shape.hpp"
#include "Rectangle.hpp"
#include "Shader.hpp"
#include <vector>
#include <random>
namespace XGF
{
	//û����ɵĹ���
	//��ʱ����
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
		float mPastTime;//��ȥ��ʱ��
		bool isAlive;//�Ƿ���Ⱦ
		float width;//
		float height;
		Point pos;
		//XMFLOAT3 density;//xyz�����ϵ��ܶ�
		int frequency;//Ƶ��ÿ���ͷ����Ӹ���
		XMFLOAT3 gravity;//���� ���� xyz
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
		ComputeShader mComputerShader;
		ComputeGPU mCgpu;
		bool firstRun;
		bool mUseCPU;//ʹ��CPU���㣿����
		unsigned int mMaxParticle;
		std::random_device rdDevice;
		std::mt19937 mtRandom;
	private:
		
	};
	enum class ParticleDevice
	{
		CPU,
		GPU,
		Auto
	};

	class ParticleFire : public ParticleSystem
	{
	public:
		class FireEmitter : public Emitter
		{
		public:
			FireEmitter();
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
			XMFLOAT3 velocity;//�ٶ� xyz����
			XMFLOAT3 acceleration;//���ٶ� xyz����
			Color color;
			bool used;
			ParticleData(Point p, Position s, Point v, Point acc, Color & c) :pos(p), velocity(v), color(c), acceleration(acc),size(s),used(true){};
		};
		void Draw();
		void Begin(const WVPMatrix & matrix);
		void End();
		void Initialize(GDI * gdi, int count = 1024, ParticleDevice particleDevice = ParticleDevice::Auto);
		void Shutdown();
		void Reset();
		void AddEmitter(FireEmitter *e) { mEmitter = e; }
		void Update(float time);
		void SetTexture(Texture *t) { mTexture = t; }

	private:
		GDI * mGDI;
		Texture *mTexture;
		FireEmitter * mEmitter;
		std::vector<ParticleData> mParticles;
		bool mFirst;
	};
}