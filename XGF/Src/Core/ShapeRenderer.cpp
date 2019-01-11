#include "..\..\Include\ShapeRenderer.hpp"
#include "..\..\Include\Rectangle.hpp"
#include "..\..\Include\RectangleC.hpp"
#include "..\..\Include\Circle.hpp"
#include "..\..\Include\Triangle.hpp"
#include "..\..\Include\Line.hpp"
#include "..\..\Include\ShaderManager.hpp"
#include "../../Include/Context.hpp"

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
		mBatch.Initialize(Context::Current().QueryShaderManager().GetBasicShaders(false, false, true), MaxVetices, MaxIndices, TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mBatch.GetShaderStage()->SetBlendState(BlendState::AddOneOneAdd);
		mBatch.GetShaderStage()->SetDepthStencilState(DepthStencilState::DepthEnable);
	}

	void ShapeRenderer::Shutdown()
	{
		mBatch.Shutdown();
	}

	void ShapeRenderer::Begin(WVPMatrix & wvp)
	{
		mBatch.GetShaderStage()->SetVSConstantBuffer(0, &wvp);
		mBatch.GetShaderStage()->SetPSConstantBuffer(0, Color(1.f, 1.f, 1.f, 1.f));

		mBatch.Begin();
	}

	void ShapeRenderer::End()
	{
		mBatch.End();
	}

	void ShapeRenderer::Flush()
	{
		mBatch.Flush();
	}

	void ShapeRenderer::DrawRectangle(float x, float y, float w, float h, float z, const Color & color)
	{
		Shape::Rectangle rc;
		rc.SetPositionAndSize(x, y, w, h);
		rc.SetZ(z);
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(rc.mPolygon->Count(), color);
		BindingBridge bb;
		bb.AddBinder(rc.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mBatch.DrawPolygon(rc.mPolygonPleIndex, bb);
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
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(rc.mPolygon->Count(), vec);

		BindingBridge bb;
		bb.AddBinder(rc.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mBatch.DrawPolygon(rc.mPolygonPleIndex, bb);
	}

	void ShapeRenderer::DrawCircle(float x, float y, float r, int precision, float z, const Color & color)
	{
		Shape::Circle ce(precision);
		ce.SetPositionAndRadius(x, y, r);
		ce.SetZ(z);
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(ce.mPolygon->Count(), color);
		BindingBridge bb;
		bb.AddBinder(ce.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mBatch.DrawPolygon(ce.mPolygonPleIndex, bb);
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
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(ce.mPolygon->Count(), vec);
		BindingBridge bb;
		bb.AddBinder(ce.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mBatch.DrawPolygon(ce.mPolygonPleIndex, bb);
	}

	void ShapeRenderer::DrawLine(float x, float y, float ex, float ey, float z, const Color & color)
	{
		Shape::Line line;
		line.SetPosition(Point(x, y, z));
		line.SetEndPosition(Point(ex, ey, z));
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(2, color);
		BindingBridge bb;
		bb.AddBinder(line.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		mBatch.DrawPolygon(line.mPolygonPleIndex, bb);
	}
	void ShapeRenderer::DrawLineList(const Position & points, int count, float z, const Color & color)
	{
		Shape::Shape shape(count, count);
		for (int i = 0; i < count; i++)
		{
			shape.mPolygon->GetData(i) = Point((&points)[i].x, (&points)[i].y, z);
			shape.mPolygonPleIndex->mIndex[i] = i;
		}
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(count, color);
		BindingBridge bb;
		bb.AddBinder(shape.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		mBatch.DrawPolygon(shape.mPolygonPleIndex, bb);
	}
	void ShapeRenderer::DrawTriangle(const Position & a, const Position & b, const Position & c, float z, const Color & ca, const Color & cb, const Color & cc)
	{
		Shape::Triangle tr;
		tr.SetThreePoint(Point(a.x, a.y, z), Point(b.x, b.y, z), Point(c.x, c.y, z));
		auto pp = std::make_shared<PolygonPleColorBinder>(3);
		BindingBridge bbr;
		bbr.AddBinder(tr.mPolygon);
		bbr.AddBinder(pp);
		pp->GetData(0) = ca;
		pp->GetData(1) = cb;
		pp->GetData(2) = cc;

		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		mBatch.DrawPolygon(tr.mPolygonPleIndex, bbr);
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
			shape.mPolygonPleIndex->mIndex[i] = i;
		}
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(count, color);
		BindingBridge bb;
		bb.AddBinder(shape.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		mBatch.DrawPolygon(shape.mPolygonPleIndex, bb);
	}
};
