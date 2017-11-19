#include "..\..\Include\EditText.hpp"
#include "..\..\Include\Layer.hpp"
#include "..\..\Include\Scene.hpp"
#include "..\..\Include\XGFramework.hpp"
#include "..\..\Include\Batch.hpp"
#include "../../Include/UIBatches.hpp"
EditText::EditText():mTextColor(Color(0.f,0.f,0.f,1.f)),mBoderColor(0.5f,0.7f,0.7f,1.0f),mInerBoderColor(0.5f,0.6f,0.6f,1.0f),mbkColor(0.2f,0.2f,0.5f,0.2f)
{
	SetMouseEventable(true);
}

EditText::~EditText()
{
	mLayer->GetParent()->GetFramework()->GetInputManager()->ClearForce(this);
}

void EditText::Render(const XMMATRIX * matrix, Batches & batches)
{
	mTextRenderer = ((UIBatches &) batches).GetTextRenderer(BATCHES_TEXTRENDERER_DEFAULT_SIZE);
	auto input = mLayer->GetParent()->GetFramework()->GetInputManager();
	BindingBridge bbr;
	int layer[3];
	PolygonPleConstantExColorBinder cb(layer, GetBorderLayer(layer));
	cb.SetLayerColor(0, mBoderColor);
	cb.SetLayerColor(1, mInerBoderColor);
	cb.SetLayerColor(2, mbkColor);
	bbr.AddBinder(cb);
	if(input->IsForce(this))
	{
		Shape::Render(*batches.GetBatch(BATCHES_BATCH_DEFAULT_PC), matrix, bbr);
		::Shape::Rectangle rc;
		GetInerBorderRectangle(rc);
		rc.SetZ(mPolygon.mPoint[0].z - 0.0001f);
		RenderText(matrix, rc, mTextColor);
	}
	else
	{
		Shape::Render(*batches.GetBatch(BATCHES_BATCH_DEFAULT_PC), matrix, bbr);
		::Shape::Rectangle rc;
		GetInerBorderRectangle(rc);
		rc.SetZ(mPolygon.mPoint[0].z - 0.0001f);
		RenderText(matrix, rc, mTextColor);
	}
	
	
}

void EditText::OnMouseDowm(const MousePoint & mp, int c, bool isIn)
{
	Actor::OnMouseDowm(mp, c, isIn);
	if(isIn)
		mLayer->GetParent()->GetFramework()->GetInputManager()->SetForce(this);
	
}

void EditText::OnMouseUp(const MousePoint & mp, int c, bool isIn)
{
	Actor::OnMouseUp(mp, c, isIn);
}

void EditText::OnMouseMove(const MousePoint & mp, int cp, bool isIn)
{
	Actor::OnMouseMove(mp, cp, isIn);
}

void EditText::OnForce(bool isForce)
{
	if (isForce)
	{
		mbkColor = Color(0.1f, 0.1f, 0.1f, 0.1f);
		mBoderColor = Color(0.8f, 0.9f, 0.9f, 1.0f);
		mInerBoderColor = Color(0.7f, 0.8f, 0.8f, 1.0f);
	}
	else
	{
		mbkColor = Color(0.2f, 0.2f, 0.5f, 0.2f);
		mBoderColor = Color(0.5f, 0.7f, 0.7f, 1.0f);
		mInerBoderColor = Color(0.5f, 0.6f, 0.6f, 1.0f);
	}
}

void EditText::GetInerBox(::Shape::Rectangle & rc)
{
	GetInerBorderRectangle(rc);
}
