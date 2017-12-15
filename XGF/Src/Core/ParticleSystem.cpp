#include "..\..\Include\ParticleSystem.hpp"
#include "..\..\Include\ConstantData.hpp"
#include "..\..\Include\ShaderConst.hpp"
#include "..\..\Include\Texture.hpp"
#include "..\..\Include\Tools.hpp"
#include "..\..\Include\GDI.hpp"
#include <algorithm>
namespace XGF
{
	using namespace DirectX;
	ParticleSystem::ParticleSystem() :mtRandom(rdDevice())
	{
	}


	ParticleSystem::~ParticleSystem()
	{
	}

	void ParticleExplosion::Begin(const WVPMatrix & matrix)
	{
		mBatch.GetShaderStage()->SetGSConstantBuffer(0, &matrix);
		auto max = XMMatrixInverse(nullptr, XMMatrixTranspose(matrix.viewMatrix));
		//max = XMMatrixTranspose(max);
		mBatch.GetShaderStage()->SetGSConstantBuffer(1, &max);
		mBatch.Begin();
	}

	void ParticleExplosion::End()
	{
		mBatch.GetShaderStage()->SetPSSRV(0, mTexture->GetShaderResourceView());
		mBatch.End();
		if (!mFirst)
		{
		}
		else
			mFirst = false;
	}

	void ParticleExplosion::Draw()
	{
		//CS4.0 cpu 模拟
		if (mEmitter->isAlive)
		{
			BindingBridge bbr;
			PolygonPleConstantColorBinder ppc(Color(0,0,0,0), 1);
			PolygonPlePoint3 pps(1);
			PolygonPlePoint4 pp4(1);
			pps.mPoint[0] = Point(5, 5, 5);
			pp4.mPoint[0] = Point4(10, 10, 4, 4);
			bbr.AddBinder(pps);
			bbr.AddBinder(pp4);
			bbr.AddBinder(ppc);
			for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
			{
				ppc.Set(0, 1, it->color);
				pps.mPoint[0] = it->pos;
				pp4.mPoint[0] = Point4(it->size.x, it->size.y, 0, 0);
				mBatch.DrawPolygon(bbr);
			}
		}
	}

	void ParticleExplosion::Initialize(GDI * gdi, int count)
	{
		mTexture = nullptr;
		mVertexShader.Initialize(gdi, ShaderConst::ParticleFireworksVS, ShaderConst::ParticleFireworksVSSize);
		mGeometryShader.Initialize(gdi, ShaderConst::ParticleFireworksGS, ShaderConst::ParticleFireworksGSSize, false);
		mPixelShader.Initialize(gdi, ShaderConst::ParticleFireworksPS, ShaderConst::ParticleFireworksPSSize);
		mShaders = { &mVertexShader, &mPixelShader, &mGeometryShader };
		mBatch.Initialize(gdi, mShaders, Emitter::mMaxParticle, 0, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		mBatch.GetShaderStage()->SetBlendState(BlendState::AddOneOneAdd);
		mBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthDisable);
		mUseCPU = true;
		mFirst = true;
	}

	void ParticleExplosion::Shutdown()
	{
		mBatch.Shutdown();
		mVertexShader.Shutdown();
		mPixelShader.Shutdown();
		mGeometryShader.Shutdown();
	}

	void ParticleExplosion::Reset()
	{
		mEmitter->mPastTime = 0;
		mEmitter->isAlive = true;
		mParticles.clear();
	}

	void ParticleExplosion::Updata(float time)
	{
		mEmitter->mPastTime += time;
		
		DirectX::XMVECTOR vecg, veca ,vecv;
		for (auto it = mParticles.begin(); it != mParticles.end(); )
		{
			vecg = DirectX::XMLoadFloat3(&mEmitter->gravity);//载入重力加速度
			veca = DirectX::XMLoadFloat3(&it->acceleration);//载入加速度
			veca = DirectX::XMVectorAdd(veca, DirectX::XMVectorScale(DirectX::XMVectorAdd(veca, vecg), time));// g 到 a 加速度变换
			DirectX::XMStoreFloat3(&it->acceleration, veca);
			vecv = DirectX::XMLoadFloat3(&it->velocity);//速度
			vecv = DirectX::XMVectorAdd(vecv, DirectX::XMVectorScale(veca , time));// a 到 v 速度变换
			DirectX::XMStoreFloat3(&it->velocity, vecv);
			float m = DirectX::XMVectorGetX(DirectX::XMVector3Length(vecv));
			veca = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&it->pos), DirectX::XMVectorScale(vecv, time));// v 到 x 位移的变换
			DirectX::XMStoreFloat3(&it->pos, veca);
			vecg = DirectX::XMLoadFloat3(&mEmitter->pos);
			vecg = DirectX::XMVectorSubtract(vecg, veca);
			//it->color-= 
			if (DirectX::XMVectorGetX(DirectX::XMVector3Length(vecg)) > mEmitter->endRadius)
				it = mParticles.erase(it);
			else
				++it;
		}
		Point p, a, v;
		int max = 0;
		float tatdt = mEmitter->frequency * time + mEmitter->delta;
		if (tatdt > 1)
		{
			max = tatdt;
			mEmitter->delta = tatdt - max;

			for (int i = 0; i < max; i++)
			{
				if (mParticles.size() >= mEmitter->mMaxParticle) break;
				std::uniform_real_distribution<double>rt(0, 1);
				std::uniform_real_distribution<double>rttn(-1, 1);
				float startag = rt(mtRandom) *(mEmitter->startAngleMax - mEmitter->startAngleMin) + mEmitter->startAngleMin, endag = rt(mtRandom) *(mEmitter->endAngleMax - mEmitter->endAngleMin) + mEmitter->endAngleMin;
				startag /= 360 * DirectX::XM_2PI;
				endag /= 360 * DirectX::XM_2PI;

				//if (startag > endag) break;//TODO::WARNING
				vecv = DirectX::XMVector3Normalize(DirectX::XMVectorSet(rttn(mtRandom), rttn(mtRandom), rttn(mtRandom), 0));
				veca = DirectX::XMVectorScale(vecv, rt(mtRandom) * (mEmitter->velocityMax - mEmitter->velocityMin) + mEmitter->velocityMin);
				vecv = DirectX::XMVectorScale(vecv, rt(mtRandom) * (mEmitter->velocityMax - mEmitter->velocityMin) + mEmitter->velocityMin);

				Color c(
					rt(mtRandom) * (mEmitter->startColorMax.x - mEmitter->startColorMin.x) + mEmitter->startColorMin.x,
					rt(mtRandom) * (mEmitter->startColorMax.y - mEmitter->startColorMin.y) + mEmitter->startColorMin.y,
					rt(mtRandom) * (mEmitter->startColorMax.z - mEmitter->startColorMin.z) + mEmitter->startColorMin.z,
					rt(mtRandom) * (mEmitter->startColorMax.w - mEmitter->startColorMin.w) + mEmitter->startColorMin.w);
				Position sv(mEmitter->width, mEmitter->height);
				if (mEmitter->mIgnoreZ)
				{
					DirectX::XMVectorMultiply(vecv, DirectX::XMVectorSet(1, 1, 0, 1));
					DirectX::XMVectorMultiply(veca, DirectX::XMVectorSet(1, 1, 0, 1));
				}
				vecg = vecv;
				vecg = DirectX::XMVector3Normalize(vecg);
				DirectX::XMStoreFloat3(&a, veca);
				DirectX::XMStoreFloat3(&v, vecv);
				DirectX::XMStoreFloat3(&p, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&mEmitter->pos), DirectX::XMVectorScale(vecg, mEmitter->startRadius)));
				mParticles.push_back(ParticleData(p, sv, v, a, c));
			}
		}
		else
			mEmitter->delta = tatdt;
	}


	Emitter::Emitter():mPastTime(0),mAliveTime(100),isAlive(true),width(3), height(3), pos(50,50,50),frequency(10), 
		mIgnoreZ(false), startAngleMin(0), startAngleMax(0), endAngleMin(360), endAngleMax(360), delta(0)
	{
		SetGravity({ 0, 0, 0 }, 1);
	}

	void Emitter::SetAliveTime(float time)
	{
		mAliveTime = time;
		mPastTime = 0;
		isAlive = true;
	}

	void Emitter::SetGravity(XMFLOAT3 g, float w)
	{
		//归一化然后缩放到w大小
		DirectX::XMStoreFloat3(&gravity, DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMVectorSet(g.x, g.y, g.z,0)), w));
	}

	void Emitter::SetFrequency(int countPerframe)
	{
		frequency = countPerframe;
	}

	ParticleExplosion::ExplosionEmitter::ExplosionEmitter():startRadius(0), endRadius(100), 
		accelerationMax(0.04), accelerationMin(0.01), velocityMax(0.03), velocityMin(0.018),
		
		startColorMin(Color(0,0,0,0)), startColorMax(Color(0, 0, 0, 0)),
		endColorMin(Color(0, 0, 0, 0)), endColorMax(Color(0, 0, 0, 0))
	{
	}

	void ParticleExplosion::ExplosionEmitter::SetColor(Color & startmin, Color & startmax, Color & endmin, Color & endmax)
	{
		startColorMax = startmax;
		startColorMin = startmin;
		endColorMax = endmax;
		endColorMin = endmin;
	}

	void ParticleExplosion::ExplosionEmitter::SetEmitterAngle(float startmin, float startmax, float endmin, float endmax)
	{
		startAngleMin = startmin;
		startAngleMax = startmax;
		endAngleMin = endmin;
		endAngleMax = endmax;
	}

};
