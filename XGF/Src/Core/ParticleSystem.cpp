#include "..\..\Include\ParticleSystem.hpp"
#include "..\..\Include\ConstantData.hpp"
#include "..\..\Include\Tools.hpp"

ParticleSystem::ParticleSystem()
{
}


ParticleSystem::~ParticleSystem()
{
}



void ParticleExplosion::Begin(const WVPMatrix & matrix)
{
	if (firstRun)
		mBatch.Begin(matrix);
	else
	{
		mBatch.Begin(matrix);
		mDrawBatch.Begin(matrix);
	}
		
}

void ParticleExplosion::End()
{
	if (firstRun)
	{
		firstRun = false;
		mBatch.End();
	}
	else
	{
		mBatch.End();
		mDrawBatch.EndWithVertexBuffer(mBatch.GetSOOutBuffer());
	}
}

void ParticleExplosion::Draw()
{
	if(mTexture !=nullptr)
		mBatch.SetTexture(*mTexture);
	BindingBridge bbr;
	PolygonPleDataBinder pdb[2];
	PolygonPlePoint3 ppl(1);
	bbr.AddBinder(ppl);
	bbr.AddBinder(pdb[0]);
	bbr.AddBinder(pdb[1]);
	for each(auto it in mEmitters)
	{
		*ppl.mPoint = it->GetPosition();
		pdb[0].mData = XMFLOAT4(it->width, it->height, it->mPastTime, it->mAliveTime);
		pdb[1].mData = XMFLOAT4(it->speed, it->radialAccel, it->startRadius, it->endRadius);
		mBatch.DrawPolygon(bbr);
	}
}

void ParticleExplosion::Initialize(GDI * gdi)
{

	ShaderLayout it[3] = { &SHADER_EL_POSITION3, &SHADER_EL_POSITION4, &SHADER_EL_POSITION4};
	EnumLayout ot[] = { SHADER_EL_SV_POSITION, SHADER_EL_TEXTURE };
	wchar_t bf[_MAX_PATH];
	GetFilePath(L"../../fx/fx/ParticleFireworks.fx", bf, MAX_PATH);
	mShader.Initialize(gdi, bf, bf, bf, it, 3, ot, 2);
	GetFilePath(L"../../fx/fx/ParticleDo.fx", bf, MAX_PATH);
	it[0] = { &SHADER_EL_SV_POSITION , true};
	it[1] = { &SHADER_EL_TEXTURE };
	mDrawShader.Initialize(gdi, bf, bf, it, 2);
	mBatch.Initialize(gdi, &mShader, 100, 0, D3D_PRIMITIVE_TOPOLOGY_POINTLIST, SOmode::SOOut);

	mDrawBatch.Initialize(gdi, &mDrawShader, 24, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, SOmode::SOIn);
	mBatch.SetZBufferRender(false);
	mBatch.SetBlend(false);
	mDrawBatch.SetZBufferRender(false);
	mDrawBatch.SetBlend(true);
	firstRun = true;
	mTexture = nullptr;
}

void ParticleExplosion::Shutdown()
{
	mDrawBatch.Shutdown();
	mBatch.Shutdown();
	mShader.Shutdown();
	mDrawShader.Shutdown();
}
