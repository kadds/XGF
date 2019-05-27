#include "..\..\Include\AxisRenderer.hpp"
#include "..\..\Include\ShaderManager.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/Renderer.hpp"
#include "../../Include/Polygon.hpp"
#include "../../Include/SystemShaders.hpp"

namespace XGF
{
	AxisRenderer::AxisRenderer()
	{
	}


	AxisRenderer::~AxisRenderer()
	{
	}

	void AxisRenderer::Initialize(float len)
	{
		mColor = std::make_shared<PolygonPleColorBinder>(12);
		mPolygon = std::make_shared<PolygonPlePointBinder>(12);
		mPolygonPleIndex = std::make_shared<PolygonPleIndex>(12);
		mPolygon->SetFullActualCount();
		mColor->SetFullActualCount();
		mPolygonPleIndex->SetFullActualCount();
		auto p = DirectX::XMMatrixIdentity();
		mRenderResource.ReCreate(SystemShaders::GetBasicShaders(SystemShaders::BasicShader_VertexColor));
		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		mRenderState.GetBlendState().GetRenderTarget(0).SetBlendEnable(false);
		SM::Color c = SM::Color(0.f, 0.f, 0.f, 1.f);
		for(int i = 0; i < 12; i++)
			mPolygonPleIndex->Get(i) = i;
		int index = 0;
		auto temp = mPolygon;
		auto fun = [&](float x, float y, float z)
		{
			temp->GetData(index++).Set(0.f, 0.f, 0.f);
			temp->GetData(index++).Set(x, y, z);
		};
		fun(len, 0.f, 0.f);
		fun(-len, 0.f, 0.f);
		fun(0.f, len, 0.f);

		fun(0.f, -len, 0.f);
		fun(0.f, 0.f, len);
		fun(0.f, 0.f, -len);
	}

	void AxisRenderer::SetAxisXColor(Color color, Color colorb)
	{
		mColor->GetData(0) = color;
		mColor->GetData(1) = color;

		mColor->GetData(2) = colorb;
		mColor->GetData(3) = colorb;
	}

	void AxisRenderer::SetAxisYColor(Color color, Color colorb)
	{
		mColor->GetData(4) = color;
		mColor->GetData(5) = color;

		mColor->GetData(6) = colorb;
		mColor->GetData(7) = colorb;
	}

	void AxisRenderer::SetAxisZColor(Color color, Color colorb)
	{
		mColor->GetData(8) = color;
		mColor->GetData(9) = color;

		mColor->GetData(10) = colorb;
		mColor->GetData(11) = colorb;
	}

	void AxisRenderer::DrawAxis()
	{
		BindingBridge bbr;
		bbr.AddBinder(mPolygon);
		bbr.AddBinder(mColor);
		int i = 0;
		mRenderResource.SetConstantBuffer<PixelShader>(0, 0, Color(1.f, 1.f, 1.f, 1.f));
		auto & renderer = Context::Current().QueryRenderer();
		renderer.Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(bbr, *mPolygonPleIndex.get(), RenderStage(mRenderState, mRenderResource)));

	}

	void AxisRenderer::Shutdown()
	{
		mPolygon.reset();
		mPolygonPleIndex.reset();
	}

	void AxisRenderer::Begin(const WVPMatrix & matrix)
	{
		mRenderResource.SetConstantBuffer<VertexShader>(0, 0, matrix);
	}

	void AxisRenderer::End()
	{
		
	}

}
