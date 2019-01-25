#include "..\..\Include\AxisRenderer.hpp"
#include "..\..\Include\ShaderManager.hpp"
#include <algorithm>
#include "../../Include/Context.hpp"
#include "../../Include/Renderer.hpp"
#include "../../Include/Polygon.hpp"

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
		auto p = DirectX::XMMatrixIdentity();
		
		mShaderStage.Initialize(Context::Current().QueryShaderManager().GetBasicShaders(false, false, true));
		mShaderStage.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		mShaderStage.SetDepthStencilState(DepthStencilState::DepthEnable);
		mShaderStage.SetBlendState(BlendState::NoneBlend);
		SM::Color c = SM::Color(0.f, 0.f, 0.f, 1.f);
		auto fun = [&](float x, float y, float z) {

			auto line = std::make_unique<Shape::Line>();
			line->SetPosition(Point(0.f, 0.f, 0.f));
			line->SetEndPosition(Point(x, y, z));
			lines.push_back(std::move(line));
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
		mColor[0] = color;
		mColor[1] = colorb;
	}

	void AxisRenderer::SetAxisYColor(Color color, Color colorb)
	{
		mColor[2] = color;
		mColor[3] = colorb;
	}

	void AxisRenderer::SetAxisZColor(Color color, Color colorb)
	{
		mColor[4] = color;
		mColor[5] = colorb;
	}

	void AxisRenderer::DrawAxis()
	{
		BindingBridge bbr;
		auto cbb = std::make_shared<PolygonPleConstantColorBinder>(2, SM::Color(0, 0, 0, 0));
		bbr.AddPlaceHolder();
		bbr.AddBinder(cbb);
		int i = 0;
		mShaderStage.SetPSConstantBuffer(0, Color(1.f, 1.f, 1.f, 1.f));
		auto & renderer = Context::Current().QueryRenderer();
		
		std::for_each(lines.begin(), lines.end(), [this, &cbb, &i, &bbr, &renderer](std::unique_ptr<Shape::Line> &var) {
			cbb->Set(0, mColor[i]);

			bbr.SetBinder(var->mPolygon, 0);
			renderer.Commit(RenderGroupType::Normal, 
				DefaultRenderCommand::MakeRenderCommand(bbr, *var->GetIndex().get(), mShaderStage));
			i++;
		});
	}

	void AxisRenderer::Shutdown()
	{
		lines.clear();
		mShaderStage.Shutdown();
	}

	void AxisRenderer::Begin(const WVPMatrix & matrix)
	{
		mShaderStage.SetVSConstantBuffer(0, &matrix);
	}

	void AxisRenderer::End()
	{
		
	}

}
