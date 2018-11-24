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

	void ParticleFire::Begin(const WVPMatrix & matrix)
	{
		mBatch.GetShaderStage()->SetGSConstantBuffer(0, &matrix);
		XMMATRIX mt = XMMatrixInverse(nullptr, matrix.viewMatrix);
		mBatch.GetShaderStage()->SetGSConstantBuffer(mBatch.GetShaderStage()->GetConstantBufferIndexByName<GeometryShader>("CBCameraPos"), &mt);
		mBatch.Begin();
	}

	void ParticleFire::End()
	{
		if (!mUseCPU)
		{
			if (!mFirst)
			{
			}
			else
				mFirst = false;
			mBatch.GetShaderStage()->SetVSSRV(0, mCgpu.GetUnorderedAccessViews(0)->srv);
			mBatch.GetShaderStage()->SetPSSRV(0, mTexture->GetRawTexture());
			mBatch.StepVertices(100);
			mBatch.End();
		}
		else
		{
			mBatch.GetShaderStage()->SetPSSRV(0, mTexture->GetRawTexture());
			mBatch.End();
			
		}
	}

	void ParticleFire::Draw()
	{
		
		if (mEmitter->isAlive)
		{
			if (!mUseCPU)
			{
				
			}
			else
			{
				BindingBridge bbr;//cpu 模拟
				auto ppc = std::make_shared<PolygonPleConstantColorBinder>(SM::Color(0, 0, 0, 0), 1);
				auto pps = std::make_shared<PolygonPlePoint3>(1);
				auto pp4 = std::make_shared<PolygonPlePoint4>(1);
				pps->mPoint[0] = Point(5, 5, 5);
				pp4->mPoint[0] = Point4(10, 10, 4, 4);
				bbr.AddBinder(pps);
				bbr.AddBinder(pp4);
				bbr.AddBinder(ppc);
				for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
				{
					ppc->Set(0, 1, it->color);
					pps->mPoint[0] = it->pos;
					pp4->mPoint[0] = Point4(it->size.x, it->size.y, 0, 0);
					mBatch.DrawPolygon(bbr);
				}
			}
			
		}
	}

	void ParticleFire::Initialize(GDI * gdi, int count, ParticleDevice device)
	{
		XGF_ASSERT(count > 0);
		mTexture = nullptr;
		if (device == ParticleDevice::Auto)
			mUseCPU = (gdi->CheckFeatureLevel() < D3D_FEATURE_LEVEL_11_0);
		else if (device == ParticleDevice::CPU)
			mUseCPU = true;
		else mUseCPU = false;
		if (!mUseCPU)//gpu
		{
			wchar_t buffer[MAX_PATH];
			Tools::GetPathBy(L"../../fx/fx/ParticleCompute.fx", buffer, MAX_PATH);
			mComputerShader.Initialize(gdi, buffer);
			mCgpu.Initialize(&mComputerShader, 10000);

			mVertexShader.Initialize(gdi, ShaderConst::ParticleComputerDoVS, ShaderConst::ParticleComputerDoVSSize);
			mGeometryShader.Initialize(gdi, ShaderConst::ParticleComputerDoGS, ShaderConst::ParticleComputerDoGSSize, false);
			mPixelShader.Initialize(gdi, ShaderConst::ParticleComputerDoPS, ShaderConst::ParticleComputerDoPSSize);
		}
		else
		{
			mVertexShader.Initialize(gdi, ShaderConst::ParticleFireworksVS, ShaderConst::ParticleFireworksVSSize);
			mGeometryShader.Initialize(gdi, ShaderConst::ParticleFireworksGS, ShaderConst::ParticleFireworksGSSize, false);
			mPixelShader.Initialize(gdi, ShaderConst::ParticleFireworksPS, ShaderConst::ParticleFireworksPSSize);
		}
		mShaders = { &mVertexShader, &mPixelShader, &mGeometryShader };
		mBatch.Initialize(gdi, mShaders, 4096, 0, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		mBatch.GetShaderStage()->SetBlendState(BlendState::AddOneOneAdd);
		mBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthDisable);
		mBatch.GetShaderStage()->SetRasterizerState(RasterizerState::SolidAndCutNone);
		mMaxParticle = count;
		mFirst = true;
	}

	void ParticleFire::Shutdown()
	{
		if (!mUseCPU)
		{
			mCgpu.Shutdown();
			mComputerShader.Shutdown();
		}

		mBatch.Shutdown();
		mVertexShader.Shutdown();
		mPixelShader.Shutdown();
		mGeometryShader.Shutdown();
	}

	void ParticleFire::Reset()
	{
		mEmitter->mPastTime = 0;
		mEmitter->isAlive = true;
		mParticles.clear();
	}

	void ParticleFire::Update(float time)
	{

		mEmitter->mPastTime += time;
		if (!mUseCPU)
		{
			mCgpu.SetCSSRV(0, mCgpu.GetUnorderedAccessViews(0)->srv);
			mCgpu.SetCSSRV(1, ConstantData::GetInstance().GetRandomSRV());
			mCgpu.Bind();
			mCgpu.Run(false);
			mCgpu.UnBind();
			auto uo = *mCgpu.GetUnorderedAccessViews(0);
			//*mCgpu.GetUnorderedAccessViews(0) = *mCgpu.GetUnorderedAccessViews(1);
			//*mCgpu.GetUnorderedAccessViews(1) = uo;//swap two uav
		}
		else
		{

			DirectX::XMVECTOR vecg, veca, vecv;
			for (auto it = mParticles.begin(); it != mParticles.end(); )
			{
				vecg = DirectX::XMLoadFloat3(&mEmitter->gravity);//载入重力加速度
				veca = DirectX::XMLoadFloat3(&it->acceleration);//载入加速度
				veca = DirectX::XMVectorAdd(veca, DirectX::XMVectorScale(DirectX::XMVectorAdd(veca, vecg), time));// g 到 a 加速度变换
				DirectX::XMStoreFloat3(&it->acceleration, veca);
				vecv = DirectX::XMLoadFloat3(&it->velocity);//速度
				vecv = DirectX::XMVectorAdd(vecv, DirectX::XMVectorScale(veca, time));// a 到 v 速度变换
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
				max = static_cast<int>(tatdt);
				mEmitter->delta = tatdt - max;

				for (int i = 0; i < max; i++)
				{
					if (mParticles.size() >= mMaxParticle) break;
					std::uniform_real_distribution<float>rt(0, 1);
					std::uniform_real_distribution<float>rttn(-1, 1);
					

					//if (startag > endag) break;//TODO::WARNING
					vecv = DirectX::XMVector3Normalize(DirectX::XMVectorSet(rttn(mtRandom), rttn(mtRandom), rttn(mtRandom), 0));
					veca = DirectX::XMVectorScale(vecv, rt(mtRandom) * (mEmitter->velocityMax - mEmitter->velocityMin) + mEmitter->velocityMin);
					vecv = DirectX::XMVectorScale(vecv, rt(mtRandom) * (mEmitter->velocityMax - mEmitter->velocityMin) + mEmitter->velocityMin);

					SM::Color c(
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
		
	}


	Emitter::Emitter():mPastTime(0),mAliveTime(100),isAlive(true),width(3), height(3), pos(50,50,50),frequency(10), 
		mIgnoreZ(false), delta(0)
	{
		SetGravity({ 0, 0, 0 }, 1);
	}

	void Emitter::SetAliveTime(float time)
	{
		mAliveTime = time;
		mPastTime = 0;
		isAlive = true;
	}

	void Emitter::SetGravity(SM::Vector3 g, float w)
	{
		//归一化然后缩放到w大小
		DirectX::XMStoreFloat3(&gravity, DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMVectorSet(g.x, g.y, g.z,0)), w));
	}

	void Emitter::SetFrequency(int countPerframe)
	{
		frequency = countPerframe;
	} 

	ParticleFire::FireEmitter::FireEmitter():startRadius(0.f), endRadius(100.f),
		accelerationMax(0.04f), accelerationMin(0.01f), velocityMax(0.03f), velocityMin(0.018f),
		
		startColorMin(SM::Color(0,0,0,0)), startColorMax(SM::Color(0, 0, 0, 0)),
		endColorMin(SM::Color(0, 0, 0, 0)), endColorMax(SM::Color(0, 0, 0, 0))
	{
	}

	void ParticleFire::FireEmitter::SetColor(SM::Color & startmin, SM::Color & startmax, SM::Color & endmin, SM::Color & endmax)
	{
		startColorMax = startmax;
		startColorMin = startmin;
		endColorMax = endmax;
		endColorMin = endmin;
	}

	void ParticleFire::FireEmitter::SetEmitterAngle(float startmin, float startmax, float endmin, float endmax)
	{
	}

};
