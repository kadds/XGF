#include "..\..\Include\ShapeRenderer.hpp"
#include "..\..\Include\ShaderConst.hpp"
#include "..\..\Include\Rectangle.hpp"
#include "..\..\Include\RectangleB.hpp"
#include "..\..\Include\Circle.hpp"
#include "..\..\Include\Line.hpp"
#include "..\..\Include\ConstantData.hpp"
ShapeRenderer::ShapeRenderer()
{
}


ShapeRenderer::~ShapeRenderer()
{
}

void ShapeRenderer::Initialize(GDI * gdi, unsigned int MaxVetices, unsigned int MaxIndices)
{
	mBatch.Initialize(gdi, ConstantData::GetInstance().GetPCShader(), MaxVetices, MaxIndices, TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mBatch.SetBlend(true);
	mBatch.SetZBufferRender(true);
}

void ShapeRenderer::Shutdown()
{
	mBatch.Shutdown();
}

void ShapeRenderer::Begin(WVPMatrix & wvp)
{
	mBatch.Begin(wvp);
}

void ShapeRenderer::End()
{
	mBatch.End();
}

void ShapeRenderer::Flush()
{
	mBatch.Flush();
}

void ShapeRenderer::DrawRectangle(float x, float y, float w, float h, float z, Color & color)
{
	Shape::Rectangle rc;
	rc.SetPositionAndSize(x, y, w, h);
	rc.SetZ(z);
	PolygonPleConstantColorBinder cb(color ,rc.mPolygon.mCount);
	BindingBridge bb;
	bb.AddBinder(rc.mPolygon);
	bb.AddBinder(cb);
	mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mBatch.DrawPolygon(rc.mPolygonPleIndex, bb);
}

void ShapeRenderer::DrawRectangleB(float x, float y, float w, float h, float z, Color & bkcolor, float borderWidth, Color & boderColor, Color & boderOuterColor)
{
	Shape::RectangleB rc;
	rc.SetPositionAndSize(x, y, w, h);
	rc.SetBorderSize(borderWidth);
	rc.SetZ(z);
	int layer[3];
	PolygonPleConstantExColorBinder cb(layer, rc.GetBorderLayer(layer));
	cb.SetLayerColor(0, boderOuterColor);
	cb.SetLayerColor(1, boderColor);
	cb.SetLayerColor(2, bkcolor);
	BindingBridge bb;
	bb.AddBinder(rc.mPolygon);
	bb.AddBinder(cb);
	mBatch.SetTexture(nullptr);
	mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mBatch.DrawPolygon(rc.mPolygonPleIndex, bb);
}

void ShapeRenderer::DrawCircle(float x, float y, float r, int precision, float z, Color & color)
{
	Shape::Circle ce(precision);
	ce.SetPosiotionAndRadius(x, y, r);
	ce.SetZ(z);
	PolygonPleConstantColorBinder cb(color, ce.mPolygon.mCount);
	BindingBridge bb;
	bb.AddBinder(ce.mPolygon);
	bb.AddBinder(cb);
	mBatch.SetTexture(nullptr);
	mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mBatch.DrawPolygon(ce.mPolygonPleIndex, bb);
}

void ShapeRenderer::DrawCircle(float x, float y, float r, int precision, float z, Color & color, Color & centerColor)
{
	Shape::Circle ce(precision);
	ce.SetPosiotionAndRadius(x, y, r);
	ce.SetZ(z);
	int layer[2];
	PolygonPleConstantExColorBinder cb(layer,ce.GetLayer(layer));
	cb.SetLayerColor(0, centerColor);
	cb.SetLayerColor(1, color);
	BindingBridge bb;
	bb.AddBinder(ce.mPolygon);
	bb.AddBinder(cb);
	mBatch.SetTexture(nullptr);
	mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mBatch.DrawPolygon(ce.mPolygonPleIndex, bb);
}

void ShapeRenderer::DrawLine(float x, float y, float ex, float ey, float z, Color & color)
{
	Shape::Line line;
	line.SetPosition(Point(x, y, z));
	line.SetEndPosition(Point(ex, ey, z));
	PolygonPleConstantColorBinder cb(color, 2);
	BindingBridge bb;
	bb.AddBinder(line.mPolygon);
	bb.AddBinder(cb);
	mBatch.SetTexture(nullptr);
	mBatch.ChangeTopologyMode(TopologyMode::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	mBatch.DrawPolygon(line.mPolygonPleIndex, bb);
}
