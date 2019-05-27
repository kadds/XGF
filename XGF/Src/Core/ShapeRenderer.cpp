#include "..\..\Include\ShapeRenderer.hpp"
#include "..\..\Include\Rectangle.hpp"
#include "..\..\Include\RectangleC.hpp"
#include "..\..\Include\Circle.hpp"
#include "..\..\Include\Triangle.hpp"
#include "..\..\Include\Line.hpp"
#include "..\..\Include\ShaderManager.hpp"
#include "../../Include/Context.hpp"
#include "../../Include/Polygon.hpp"
#include "../../Include/AxisRenderer.hpp"
#include "../../Include/Renderer.hpp"
#include "../../Include/SystemShaders.hpp"

namespace XGF
{
	ShapeRenderer::ShapeRenderer()
	{
	}


	ShapeRenderer::~ShapeRenderer()
	{
	}

	void ShapeRenderer::Initialize(unsigned int MaxVetices, unsigned int MaxIndices)
	{
		mRenderResource.ReCreate(SystemShaders::GetBasicShaders(SystemShaders::BasicShader_VertexColor));
	}

	void ShapeRenderer::Shutdown()
	{
		mRenderResource.Clear();
	}

	void ShapeRenderer::Begin(WVPMatrix & wvp)
	{
		mRenderResource.SetConstantBuffer<VertexShader>(0, 0, wvp);
		mRenderResource.SetConstantBuffer<PixelShader>(0, 0, Color(1.f, 1.f, 1.f, 1.f));
	}

	void ShapeRenderer::End()
	{
	}

	void ShapeRenderer::Flush()
	{
	}

	void ShapeRenderer::DrawRectangle(float x, float y, float w, float h, float z, const Color & color)
	{
		Shape::Rectangle rc;
		rc.SetPositionAndSize(x, y, w, h);
		rc.SetZ(z);
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(rc.mPolygon->GetActualCount(), color);
		cb->SetFullActualCount();
		BindingBridge bb;
		bb.AddBinder(rc.mPolygon);
		bb.AddBinder(cb);
		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(
			bb, *rc.mPolygonPleIndex.get(), RenderStage(mRenderState, mRenderResource)));
	}

	void ShapeRenderer::DrawRectangleC(float x, float y, float w, float h, float z, const Color & bkcolor, float borderWidth, const Color & boderColor, const Color & boderOuterColor)
	{
		Shape::RectangleC rc;
		rc.SetPositionAndSize(x, y, w, h);
		rc.SetBorderSize(borderWidth);
		rc.SetZ(z);
		std::vector<std::pair<int, Color>> vec = 
		{ 
			std::make_pair(0, boderOuterColor), 
			std::make_pair(4, boderColor),
			std::make_pair(8, bkcolor)
		};
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(rc.mPolygon->GetActualCount(), vec);
		cb->SetFullActualCount();
		BindingBridge bb;
		bb.AddBinder(rc.mPolygon);
		bb.AddBinder(cb);
		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(
			bb, *rc.mPolygonPleIndex.get(), RenderStage(mRenderState, mRenderResource)));
	}

	void ShapeRenderer::DrawCircle(float x, float y, float r, int precision, float z, const Color & color)
	{
		Shape::Circle ce(precision);
		ce.SetPositionAndRadius(x, y, r);
		ce.SetZ(z);
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(ce.mPolygon->GetActualCount(), color);
		cb->SetFullActualCount();
		BindingBridge bb;
		bb.AddBinder(ce.mPolygon);
		bb.AddBinder(cb);
		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(
			bb, *ce.mPolygonPleIndex.get(), RenderStage(mRenderState, mRenderResource)));
	}

	void ShapeRenderer::DrawCircle(float x, float y, float r, int precision, float z, const Color & color, const Color & centerColor)
	{
		Shape::Circle ce(precision);
		ce.SetPositionAndRadius(x, y, r);
		ce.SetZ(z);
		std::vector<std::pair<int, Color>> vec =
		{
			std::make_pair(0, centerColor),
			std::make_pair(1, color),
		};
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(ce.mPolygon->GetActualCount(), vec);
		cb->SetFullActualCount();
		BindingBridge bb;
		bb.AddBinder(ce.mPolygon);
		bb.AddBinder(cb);
		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(
			bb, *ce.mPolygonPleIndex.get(), RenderStage(mRenderState, mRenderResource)));
	}

	void ShapeRenderer::DrawLine(float x, float y, float ex, float ey, float z, const Color & color)
	{
		Shape::Line line;
		line.SetPosition(Point(x, y, z));
		line.SetEndPosition(Point(ex, ey, z));
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(2, color);
		cb->SetFullActualCount();
		BindingBridge bb;
		bb.AddBinder(line.mPolygon);
		bb.AddBinder(cb);
		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(
			bb, *line.mPolygonPleIndex.get(), RenderStage(mRenderState, mRenderResource)));
	}
	void ShapeRenderer::DrawLineList(const Position & points, int count, float z, const Color & color)
	{
		Shape::Shape shape(count, count);
		for (int i = 0; i < count; i++)
		{
			shape.mPolygon->GetData(i) = Point((&points)[i].x, (&points)[i].y, z);
			shape.mPolygonPleIndex->Get(i) = i;
		}
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(count, color);
		cb->SetFullActualCount();
		BindingBridge bb;
		bb.AddBinder(shape.mPolygon);
		bb.AddBinder(cb);
		shape.mPolygon->SetActualCount(count);
		cb->SetActualCount(count);
		shape.mPolygonPleIndex->SetActualCount(count);
		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(
			bb, *shape.mPolygonPleIndex.get(), RenderStage(mRenderState, mRenderResource)));
	}
	void ShapeRenderer::DrawTriangle(const Position & a, const Position & b, const Position & c, float z, const Color & ca, const Color & cb, const Color & cc)
	{
		Shape::Triangle tr;
		tr.SetThreePoint(Point(a.x, a.y, z), Point(b.x, b.y, z), Point(c.x, c.y, z));
		auto pp = std::make_shared<PolygonPleColorBinder>(3);
		pp->SetFullActualCount();
		BindingBridge bbr;
		bbr.AddBinder(tr.mPolygon);
		bbr.AddBinder(pp);
		pp->GetData(0) = ca;
		pp->GetData(1) = cb;
		pp->GetData(2) = cc;

		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(
			bbr, *tr.mPolygonPleIndex.get(), RenderStage(mRenderState, mRenderResource)));
	}
	void ShapeRenderer::DrawTriangle(const Position & a, const Position & b, const Position & c, float z, const Color & cc)
	{
		DrawTriangle(a, b, c, z, cc, cc, cc);
	}
	void ShapeRenderer::DrawPolygonList(const Position & points, int count, float z, const Color & color)
	{
		Shape::Shape shape(count, count);
		for (int i = 0; i < count; i++)
		{
			shape.mPolygon->GetData(i) = Point((&points)[i].x, (&points)[i].y, z);
			shape.mPolygonPleIndex->Get(i) = i;
		}
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(count, color);
		cb->SetFullActualCount();
		shape.mPolygon->SetFullActualCount();
		shape.mPolygonPleIndex->SetFullActualCount();
		BindingBridge bb;
		bb.AddBinder(shape.mPolygon);
		bb.AddBinder(cb);
		mRenderState.SetTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		Context::Current().QueryRenderer().Commit(RenderGroupType::Normal, DefaultRenderCommand::MakeRenderCommand(
			bb, *shape.mPolygonPleIndex.get(), RenderStage(mRenderState, mRenderResource)));
	}
};
