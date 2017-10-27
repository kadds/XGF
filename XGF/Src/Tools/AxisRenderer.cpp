#include "..\..\Include\AxisRenderer.hpp"
#include "../../Include/ShaderConst.hpp"
#include <algorithm>
AxisRenderer::AxisRenderer()
{
}


AxisRenderer::~AxisRenderer()
{
}

void AxisRenderer::Initialize(GDI * gdi)
{
	auto p = DirectX::XMMatrixIdentity();
	InputType inputype[2] = { SHADER_INPUTLAYOUT_POSITION, SHADER_INPUTLAYOUT_COLOR};
	mShader.Initialize(gdi, ShaderConst::shaderPCVS, ShaderConst::shaderPCVSSize, ShaderConst::shaderPCPS, ShaderConst::shaderPCPSSize, inputype, 2);
	mBatch.Initialize(gdi, &mShader,  2 * 3 * 2,  2 * 3 * 2, TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	mBatch.SetBlend(true);
	mBatch.SetZBufferRender(false);
	Color c = Color(0.f, 0.f, 0.f, 1.f);
	auto fun = [&](float x, float y, float z) {
		
		auto line = std::make_unique<Shape::Line>();
		line->SetPosition(Point(0.f, 0.f, 0.f));
		line->SetEndPosition(Point(x , y, z));
		lines.push_back(std::move(line));
		//mBatch.WriteToVertexBuffer(0, pos, line->mPolygon.mPoint, 2, VertexTransfrom::None);
		//mBatch.WriteToVertexBuffer(1, pos, &c,  2, VertexTransfrom::Constant);
	};
	fun(1e8f,0.f,0.f);
	fun(-1e8f, 0.f, 0.f);
	fun(0.f, 1e8f, 0.f);

	fun(0.f, -1e8f, 0.f);
	fun(0.f, 0.f, 1e8f);
	fun(0.f, 0.f, -1e8f);
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
	PolygonPleConstantColorBinder cbb(Color(0,0,0,0),2);
	bbr.AddBinder(cbb);
	int i = 0;
	std::for_each(lines.begin(), lines.end(), [this,&cbb,&i,&bbr](std::unique_ptr<Shape::Line> &var) {
		cbb.Set(0, 1, mColor[i]);
		mBatch.DrawPolygon(var->mPolygon, var->mPolygonPleIndex, bbr);
		i++;
	});
}

void AxisRenderer::Shutdown()
{
	lines.clear();
	mBatch.Shutdown();
	mShader.Shutdown();
}

void AxisRenderer::Begin(const WVPMatrix & matrix)
{
	mBatch.Begin(matrix);
}

void AxisRenderer::End()
{
	mBatch.End();
}
