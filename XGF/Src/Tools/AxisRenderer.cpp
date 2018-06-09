#include "..\..\Include\AxisRenderer.hpp"
#include "../../Include/ShaderConst.hpp"
#include "..\..\Include\ConstantData.hpp"
#include <algorithm>
namespace XGF
{
	AxisRenderer::AxisRenderer()
	{
	}


	AxisRenderer::~AxisRenderer()
	{
	}

	void AxisRenderer::Initialize(GDI * gdi, float len)
	{
		auto p = DirectX::XMMatrixIdentity();
		mBatch.Initialize(gdi, ConstantData::GetInstance().GetPCShaders(), 2 * 3 * 2, 2 * 3 * 2, TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		mBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthEnable);
		mBatch.GetShaderStage()->SetBlendState(BlendState::NoneBlend);
		
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

	void AxisRenderer::SetAxisXColor(SM::Color color, SM::Color colorb)
	{
		mColor[0] = color;
		mColor[1] = colorb;
	}

	void AxisRenderer::SetAxisYColor(SM::Color color, SM::Color colorb)
	{
		mColor[2] = color;
		mColor[3] = colorb;
	}

	void AxisRenderer::SetAxisZColor(SM::Color color, SM::Color colorb)
	{
		mColor[4] = color;
		mColor[5] = colorb;
	}

	void AxisRenderer::DrawAxis()
	{
		BindingBridge bbr;
		PolygonPleConstantColorBinder cbb(SM::Color(0, 0, 0, 0), 2);
		bbr.AddPlaceHolder();
		bbr.AddBinder(cbb);
		int i = 0;
		std::for_each(lines.begin(), lines.end(), [this, &cbb, &i, &bbr](std::unique_ptr<Shape::Line> &var) {
			cbb.Set(0, 1, mColor[i]);
			bbr.SetBinder(var->mPolygon, 0);
			mBatch.DrawPolygon(var->mPolygonPleIndex, bbr);
			i++;
		});
	}

	void AxisRenderer::Shutdown()
	{
		lines.clear();
		mBatch.Shutdown();
	}

	void AxisRenderer::Begin(const WVPMatrix & matrix)
	{
		mBatch.GetShaderStage()->SetVSConstantBuffer(0, &matrix);
		mBatch.Begin();
	}

	void AxisRenderer::End()
	{
		mBatch.End();
	}

}
