#pragma once
#include "Batch.hpp"
#include "Shape.hpp"
#include "Rectangle.hpp"
#include "Shader.hpp"
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
	bool firstRun;
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
	void Draw();
	void Begin(const WVPMatrix & matrix);
	void End();
	void Initialize(GDI * gdi);
	void Shutdown();
	void AddEmitter(ExplosionEmitter *e) { mEmitters.push_back(e); }
private:
	Texture *mTexture;
	std::vector<ExplosionEmitter *> mEmitters;
};