#include "..\..\Include\EditText.hpp"
#include "..\..\Include\Scene.hpp"
#include "..\..\Include\XGFramework.hpp"
#include "..\..\Include\Batch.hpp"
#include "../../Include/UIBatches.hpp"
namespace XGF
{
	EditText::EditText() :mTextColor(Color(0.f, 0.f, 0.f, 1.f)), mBoderColor(0.5f, 0.7f, 0.7f, 1.0f), mInerBoderColor(0.5f, 0.6f, 0.6f, 1.0f), mbkColor(0.2f, 0.2f, 0.5f, 0.2f)
	{
	}

	EditText::~EditText()
	{
		//mParent->GetScene()->GetFramework()->GetInputManager()->ClearForce(this);
	}

	void EditText::Render(const XMMATRIX * matrix)
	{
		mTextRenderer = this->mParent->GetScene()->GetFramework()->GetUIBatches().GetTextRenderer(BATCHES_TEXTRENDERER_DEFAULT_SIZE);
		auto input = mParent->GetScene()->GetFramework()->GetInputManager();
		BindingBridge bbr;
		int layer[3];
		PolygonPleConstantExColorBinder cb(layer, GetBorderLayer(layer));
		cb.SetLayerColor(0, mBoderColor);
		cb.SetLayerColor(1, mInerBoderColor);
		cb.SetLayerColor(2, mbkColor);
		bbr.AddBinder(cb);
		if (input->IsForce(this))
		{
			::XGF::Shape::Shape::Render(*this->mParent->GetScene()->GetFramework()->GetUIBatches().GetBatch(BATCHES_BATCH_DEFAULT_PC), matrix, bbr);
			::XGF::Shape::Rectangle rc;
			GetInerBorderRectangle(rc);
			rc.SetZ(mPolygon.mPoint[0].z - 0.0001f);
			RenderText(matrix, rc, mTextColor);
		}
		else
		{
			::XGF::Shape::Shape::Render(*this->mParent->GetScene()->GetFramework()->GetUIBatches().GetBatch(BATCHES_BATCH_DEFAULT_PC), matrix, bbr);
			::XGF::Shape::Rectangle rc;
			GetInerBorderRectangle(rc);
			rc.SetZ(mPolygon.mPoint[0].z - 0.0001f);
			RenderText(matrix, rc, mTextColor);
		}


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

	void EditText::GetInerBox(::XGF::Shape::Rectangle & rc)
	{
		GetInerBorderRectangle(rc);
	}

	void EditText::OnAddToContainer()
	{
		Control::OnAddToContainer();
		
	}

	void EditText::OnRemoveFromContainer()
	{
		Control::OnRemoveFromContainer();
		mParent->GetScene()->GetFramework()->GetInputManager()->ClearForce(this);
	}

	void EditText::OnMouseDown(const Event & ev)
	{
		Control::OnMouseDown(ev);
		if (this->IsInBoundBox(Point(ev.GetDataInt(0), ev.GetDataInt(1), 0.f),mTransform.GetMatrix()))
		{
			mParent->GetScene()->GetFramework()->GetInputManager()->SetForce(this);
		}
	}

}
