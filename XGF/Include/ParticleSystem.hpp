#pragma once
#include "Batch.hpp"
#include "Shape.hpp"
#include "Rectangle.hpp"
#include "Shader.hpp"
class Particle
{
public:
	virtual void Draw(Batch & bt) = 0;
protected:

private:
};
class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();
	void Initialize(GDI * gdi, int MaxCount);
	void Shutdown();

	void Draw(Particle & pt);
	void Begin(const WVPMatrix & matrix);
	void End();
	void Flush();
private:
	Batch mBatch;
	Shader mShader;
};

class PointColorParticle : public Particle
{
	class ParticleData
	{
	public:
		bool alive;
		Point pos;
		Color color;
	};
public:
	virtual void Draw(Batch & bt);
	void Update();
	void SetPosition(Point p) { pos = p; };
protected:

private:
	int maxcount;
	ParticleData allparticle[500];
	Point pos;
};
class RectangleColorParticle
{
public:
	virtual void Draw(Batch & bt);
	//void SetPosition();
protected:

private:
	Shape::Rectangle rc;
};
