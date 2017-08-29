#include "..\..\Include\ShapeRenderer.hpp"
#include "..\..\Include\ShaderConst.hpp"
#include "..\..\Include\Rectangle.hpp"
#include "..\..\Include\RectangleB.hpp"
#include "..\..\Include\Circle.hpp"
ShapeRenderer::ShapeRenderer()
{
}


ShapeRenderer::~ShapeRenderer()
{
}

void ShapeRenderer::Initialize(GDI * gdi, unsigned int MaxVetices, unsigned int MaxIndices)
{
	InputType inputtype[2] = { SHADER_INPUTLAYOUT_POSITION, SHADER_INPUTLAYOUT_COLOR };
	mShader.Initialize(gdi, ShaderConst::shaderPCVS, ShaderConst::shaderPCVSSize, ShaderConst::shaderPCPS, ShaderConst::shaderPCPSSize, inputtype, 2);
	mBatch.Initialize(gdi, &mShader, MaxVetices, MaxIndices,RealTime,false);

}

void ShapeRenderer::Shutdown()
{
	mBatch.Shutdown();
	mShader.Shutdown();
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

void ShapeRenderer::DrawRectangle(float x, float y, float w, float h, Color & color)
{
	Shape::Rectangle rc;
	rc.SetPositionAndSize(x, y, w, h);
	rc.mPolygon.Transform(Batch::GetClientWidth() / 2, Batch::GetClientHeight() / 2);
	PolygonPleConstantColorBinder cb(color ,rc.mPolygon.mCount);
	BindingBridge bb;
	bb.AddBinder(cb);
	mBatch.DrawPolygon(rc.mPolygon, rc.mPolygonPleIndex, bb);
}

void ShapeRenderer::DrawRectangleB(float x, float y, float w, float h, Color & bkcolor, float borderWidth, Color & boderColor, Color & boderOuterColor)
{
	Shape::RectangleB rc;
	rc.SetPositionAndSize(x, y, w, h);
	rc.SetBorderSize(borderWidth);
	rc.mPolygon.Transform(Batch::GetClientWidth() / 2, Batch::GetClientHeight() / 2);
	int layer[3];
	PolygonPleConstantExColorBinder cb(layer, rc.GetBorderLayer(layer));
	cb.SetLayerColor(0, boderOuterColor);
	cb.SetLayerColor(1, boderColor);
	cb.SetLayerColor(2, bkcolor);
	BindingBridge bb;
	bb.AddBinder(cb);
	mBatch.SetTexture(nullptr);
	mBatch.DrawPolygon(rc.mPolygon, rc.mPolygonPleIndex, bb);
}

void ShapeRenderer::DrawCircle(float x, float y, float r, int precision, Color & color)
{
	Shape::Circle ce(precision);
	ce.SetPosiotionAndRadius(x, y, r);
	ce.mPolygon.Transform(Batch::GetClientWidth() / 2, Batch::GetClientHeight() / 2);
	PolygonPleConstantColorBinder cb(color, ce.mPolygon.mCount);
	BindingBridge bb;
	bb.AddBinder(cb);
	mBatch.SetTexture(nullptr);
	mBatch.DrawPolygon(ce.mPolygon, ce.mPolygonPleIndex, bb);
}

void ShapeRenderer::DrawCircle(float x, float y, float r, int precision, Color & color, Color & centerColor)
{
	Shape::Circle ce(precision);
	ce.SetPosiotionAndRadius(x, y, r);
	ce.mPolygon.Transform(Batch::GetClientWidth() / 2, Batch::GetClientHeight() / 2);
	int layer[2];
	PolygonPleConstantExColorBinder cb(layer,ce.GetLayer(layer));
	cb.SetLayerColor(0, centerColor);
	cb.SetLayerColor(1, color);
	BindingBridge bb;
	bb.AddBinder(cb);
	mBatch.SetTexture(nullptr);
	mBatch.DrawPolygon(ce.mPolygon, ce.mPolygonPleIndex, bb);
}
