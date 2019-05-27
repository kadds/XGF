#include "./../../Include/ShadowGenerator.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/Renderer.hpp"
#include "../../Include/ShaderManager.hpp"
#include "../../Include/SystemShaders.hpp"
#include "../../Include/Light/Light.hpp"

namespace XGF::Shape
{
	ShadowMapGenerator::ShadowMapGenerator(): mRenderResource(
		SystemShaders::GetShadowMapShaders())
	{
		mRenderState.GetViewPorts().emplace_back();
	}

	void ShadowMapGenerator::Initialize(int width, int height)
	{
		mFrameBuffer.Initialize(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DepthStencilFormat::Depth24FloatStencil8Uint, 0, 0, true);
		mRenderState.GetViewPorts()[0] = ViewPort(0.f, 0.f, mFrameBuffer.GetWidth(), mFrameBuffer.GetHeight(), 0.f, 1.f);
	}

	void ShadowMapGenerator::Config(int width, int height)
	{
		mFrameBuffer.Shutdown();
		mFrameBuffer.Initialize(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DepthStencilFormat::Depth24FloatStencil8Uint, 0, 0, true);
		mRenderState.GetViewPorts()[0] = ViewPort(0.f, 0.f, mFrameBuffer.GetWidth(), mFrameBuffer.GetHeight(), 0.f, 1.f);
	}
	Texture * ShadowMapGenerator::Generate(CastShadowAbleLight * light, const std::vector<Mesh*>& meshs, const LightShadowMatrix & wvp)
	{
		BindingBridge bbr;
		auto & renderer = Context::Current().QueryRenderer();
		renderer.PushFrameTarget(&mFrameBuffer);
		auto & raster = mRenderState.GetRasterizerState();
		raster.SetSlopeScaledDepthBias(light->GetSlopeScaledDepthBias());
		raster.SetDepthBias(light->GetDepthBias());
		raster.SetDepthBiasClamp(light->GetDepthBiasClamp());
		renderer.ClearDepthStencilBuffer();
		mRenderResource.SetConstantBuffer<VertexShader>(0, 0, wvp);
		mRenderState.GetDepthStencilState().SetDepthFunc(ComparisonFunc::LESS_EQUAL);
		for (auto mesh : meshs)
		{
			if (!mesh->GetCastShadow()) continue;
			bbr.Clear();
			auto ppe = std::make_shared<PolygonPlePointBinder>(mesh->GetGeometry()->GetVertexBinder()->GetActualCount());
			auto matrix = mesh->GetGeometry()->GetTransform().GetMatrix();
			mesh->GetGeometry()->mPolygon->ExpandAllTo(*ppe.get(), Operator::Multiply(matrix));
			bbr.AddBinder(ppe);
			renderer.Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(bbr, *mesh->GetGeometry()->GetIndex().get(), RenderStage(mRenderState, mRenderResource)));
		}
		renderer.PopNewFrameTarget();
		return mFrameBuffer.GetDepthStencilTexture();
	}

	void ShadowMapGenerator::Shutdown()
	{
		mFrameBuffer.Shutdown();
	}

	Texture* ShadowMapGenerator::GetTexture()
	{
		return mFrameBuffer.GetDepthStencilTexture();
	}
}
