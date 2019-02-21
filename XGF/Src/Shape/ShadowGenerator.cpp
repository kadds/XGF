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
	}

	void ShadowMapGenerator::Initialize(int width, int height)
	{
		mFrameBuffer.Initialize(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, true);
	}

	void ShadowMapGenerator::Config(int width, int height)
	{
		mFrameBuffer.Shutdown();
		mFrameBuffer.Initialize(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 0, true);
	
	}
	Texture * ShadowMapGenerator::Generate(CastShadowAbleLight * light, const std::vector<Mesh*>& meshs, const LightShadowMatrix & wvp)
	{
		BindingBridge bbr;
		auto & renderer = Context::Current().QueryRenderer();
		auto * renderTarget = renderer.GetCurrentFrameTarget();
		renderer.AppendAndSetFrameTarget(&mFrameBuffer);
		auto & raster = mRenderState.GetRasterizerState();
		raster.SetSlopeScaledDepthBias(light->GetSlopeScaledDepthBias());
		renderer.ClearDepthStencilBuffer();
		mRenderResource.SetConstantBuffer<VertexShader>(0, 0, wvp);
		mRenderState.GetDepthStencilState().SetDepthFunc(ComparisonFunc::LESS_EQUAL);
		for (auto mesh : meshs)
		{
			if (!mesh->GetCastShadow()) continue;
			bbr.Clear();
			auto ppe = std::make_shared<PolygonPlePointBinder>(mesh->GetGeometry()->GetVertexBinders()[0]->GetActualCount());
			auto matrix = mesh->GetGeometry()->GetTransform().GetMatrix();
			mesh->GetGeometry()->mPolygon->ExpandAllTo(*ppe.get(), Operator::Multiply(matrix));
			bbr.AddBinder(ppe);
			renderer.Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(bbr, *mesh->GetGeometry()->GetIndex().get(), RenderStage(mRenderState, mRenderResource)));
		}
		renderer.SetFrameTarget(renderTarget);
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
