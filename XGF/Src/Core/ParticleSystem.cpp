#include "..\..\Include\ParticleSystem.hpp"
#include "..\..\Include\ConstantData.hpp"
#include "..\..\Include\Tools.hpp"

ParticleSystem::ParticleSystem()
{
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Initialize(GDI * gdi, int MaxCount)
{
	InputType it = SHADER_INPUTLAYOUT_POSITION;
	wchar_t bf[_MAX_PATH];
	GetFilePath(L"../../fx/fx/gs.fx", bf, MAX_PATH);
	mShader.Initialize(gdi, bf, bf, bf, &it, 1);
	mBatch.Initialize(gdi, &mShader, MaxCount, 0, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void ParticleSystem::Shutdown()
{
	mBatch.Shutdown();
	mShader.Shutdown();
}


void ParticleSystem::Draw(Particle & pt)
{
	pt.Draw(mBatch);
}

void ParticleSystem::Begin(const WVPMatrix & matrix)
{
	mBatch.Begin(matrix);
}

void ParticleSystem::End()
{
	mBatch.End();
}

void ParticleSystem::Flush()
{
	mBatch.Flush();
}

void PointColorParticle::Draw(Batch & bt)
{
	PolygonPle p(1);
	BindingBridge bbr;
	PolygonPleColorBinder cb(1);
	for (int i = 0;i < 100; i ++)
	{
		if (allparticle[i].alive)
		{
			p.mPoint[0] = allparticle[i].pos;
			
			bt.DrawPolygon(p, bbr);
		}
	}
	
}

void PointColorParticle::Update()
{
	for (int i = 0;i < 100; i++)
	{
		allparticle[i].alive = true;
		
		allparticle[i].pos = Point(0,i/100.0,i/200.0);

		allparticle[i].color = Color(1.0 * i / 100.0, 0,0.4,1.0);
	}
}

void RectangleColorParticle::Draw(Batch & bt)
{
}
