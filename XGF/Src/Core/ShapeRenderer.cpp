#include "..\..\Include\ShapeRenderer.hpp"
#include "..\..\Include\Rectangle.hpp"
#include "..\..\Include\RectangleC.hpp"
#include "..\..\Include\Circle.hpp"
#include "..\..\Include\Triangle.hpp"
#include "..\..\Include\Line.hpp"
#include "..\..\Include\ConstantData.hpp"
namespace XGF
{
	ShapeRenderer::ShapeRenderer()
	{
	}


	ShapeRenderer::~ShapeRenderer()
	{
	}

	void ShapeRenderer::Initialize(GDI * gdi, unsigned int MaxVetices, unsigned int MaxIndices)
	{
		mBatch.Initialize(gdi, ConstantData::GetInstance().GetPCShaders(), MaxVetices, MaxIndices, TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

	void ShapeRenderer::DrawRectangle(float x, float y, float w, float h, float z, const SM::Color & color)
	{
		Shape::Rectangle rc;
		rc.SetPositionAndSize(x, y, w, h);
		rc.SetZ(z);
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(color, rc.mPolygon->mCount);
		BindingBridge bb;
		bb.AddBinder(rc.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mBatch.DrawPolygon(rc.mPolygonPleIndex, bb);
	}

	void ShapeRenderer::DrawRectangleC(float x, float y, float w, float h, float z, const SM::Color & bkcolor, float borderWidth, const SM::Color & boderColor, const SM::Color & boderOuterColor)
	{
		Shape::RectangleC rc;
		rc.SetPositionAndSize(x, y, w, h);
		rc.SetBorderSize(borderWidth);
		rc.SetZ(z);
		int layer[3];
		auto cb = std::make_shared<PolygonPleConstantExColorBinder>(layer, rc.GetBorderLayer(layer));
		cb->SetLayerColor(0, boderOuterColor);
		cb->SetLayerColor(1, boderColor);
		cb->SetLayerColor(2, bkcolor);
		BindingBridge bb;
		bb.AddBinder(rc.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mBatch.DrawPolygon(rc.mPolygonPleIndex, bb);
	}

	void ShapeRenderer::DrawCircle(float x, float y, float r, int precision, float z, const SM::Color & color)
	{
		Shape::Circle ce(precision);
		ce.SetPosiotionAndRadius(x, y, r);
		ce.SetZ(z);
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(color, ce.mPolygon->mCount);
		BindingBridge bb;
		bb.AddBinder(ce.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mBatch.DrawPolygon(ce.mPolygonPleIndex, bb);
	}

	void ShapeRenderer::DrawCircle(float x, float y, float r, int precision, float z, const SM::Color & color, const SM::Color & centerColor)
	{
		Shape::Circle ce(precision);
		ce.SetPosiotionAndRadius(x, y, r);
		ce.SetZ(z);
		int layer[2];
		auto cb = std::make_shared<PolygonPleConstantExColorBinder>(layer, ce.GetLayer(layer));
		cb->SetLayerColor(0, centerColor);
		cb->SetLayerColor(1, color);
		BindingBridge bb;
		bb.AddBinder(ce.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		mBatch.DrawPolygon(ce.mPolygonPleIndex, bb);
	}

	void ShapeRenderer::DrawLine(float x, float y, float ex, float ey, float z, const SM::Color & color)
	{
		Shape::Line line;
		line.SetPosition(Point(x, y, z));
		line.SetEndPosition(Point(ex, ey, z));
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(color, 2);
		BindingBridge bb;
		bb.AddBinder(line.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		mBatch.DrawPolygon(line.mPolygonPleIndex, bb);
	}
	void ShapeRenderer::DrawLineList(const Position & points, int count, float z, const SM::Color & color)
	{
		Shape::Shape shape(count, count);
		for (int i = 0; i < count; i++)
		{
			shape.mPolygon->mPoint[i] = Point((&points)[i].x, (&points)[i].y, z);
			shape.mPolygonPleIndex->mIndex[i] = i;
		}
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(color, count);
		BindingBridge bb;
		bb.AddBinder(shape.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		mBatch.DrawPolygon(shape.mPolygonPleIndex, bb);
	}
	void ShapeRenderer::DrawTriangle(const Position & a, const Position & b, const Position & c, float z, const SM::Color & ca, const SM::Color & cb, const SM::Color & cc)
	{
		Shape::Triangle tr;
		tr.SetThreePoint(Point(a.x, a.y, z), Point(b.x, b.y, z), Point(c.x, c.y, z));
		auto pp = std::make_shared<PolygonPleColorBinder>(3);
		BindingBridge bbr;
		bbr.AddBinder(tr.mPolygon);
		bbr.AddBinder(pp);
		pp->Set(0, 1, ca);
		pp->Set(1, 1, cb);
		pp->Set(2, 1, cc);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		mBatch.DrawPolygon(tr.mPolygonPleIndex, bbr);
	}
	void ShapeRenderer::DrawTriangle(const Position & a, const Position & b, const Position & c, float z, const SM::Color & cc)
	{
		DrawTriangle(a, b, c, z, cc, cc, cc);
	}
	void ShapeRenderer::DrawPolygonList(const Position & points, int count, float z, const SM::Color & color)
	{
		Shape::Shape shape(count, count);
		for (int i = 0; i < count; i++)
		{
			shape.mPolygon->mPoint[i] = Point((&points)[i].x, (&points)[i].y, z);
			shape.mPolygonPleIndex->mIndex[i] = i;
		}
		auto cb = std::make_shared<PolygonPleConstantColorBinder>(color, count);
		BindingBridge bb;
		bb.AddBinder(shape.mPolygon);
		bb.AddBinder(cb);
		mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		mBatch.DrawPolygon(shape.mPolygonPleIndex, bb);
	}
};
