#include "../../Include/Control.hpp"
#include "../../Include/Container.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/XGFramework.hpp"
namespace XGF
{
	Control::Control() :Actor(), mFontSize(FontSize::Default), mSkin(nullptr), mNowState(SkinState::normal)
	{
	}

	Control::Control(int id) : Actor(id), mFontSize(FontSize::Default), mSkin(nullptr), mNowState(SkinState::normal)
	{
	}

	Control::~Control()
	{
	}

	std::shared_ptr<Skin> Control::SetSkin(std::shared_ptr<Skin> newskin)
	{
		auto old = mSkin;
		mSkin = newskin;
		return old;
	}

	void Control::OnMouseDown(const Event & ev)
	{
		bool isInControl = GetShape()->IsInBoundBox(Point(ev.GetDataInt(0), (float)ev.GetDataInt(1), 0.f), mTransform.GetMatrix());
		if (isInControl)
		{
			mNowState = SkinState::active;
		}
		else
		{
			mNowState = SkinState::normal;
		}
		mClickHelper.OnMouseDowm({ ev.GetDataInt(0), ev.GetDataInt(1) }, isInControl);
	}

	void Control::OnMouseUp(const Event & ev)
	{
		bool isInControl = GetShape()->IsInBoundBox(Point(ev.GetDataInt(0), (float)ev.GetDataInt(1), 0.f), mTransform.GetMatrix());
		if (isInControl)
		{
			mNowState = SkinState::hover;
		}
		mClickHelper.OnMouseUp({ ev.GetDataInt(0), ev.GetDataInt(1) }, isInControl);
	}

	void Control::OnMouseMove(const Event & ev)
	{
		bool isInControl = GetShape()->IsInBoundBox(Point(ev.GetDataInt(0), (float)ev.GetDataInt(1), 0.f), mTransform.GetMatrix());
		if (isInControl)
		{
			if(mNowState != SkinState::active)
				mNowState = SkinState::hover;
		}
		else
		{
			mNowState = SkinState::normal;
		}
		mClickHelper.OnMouseMove(isInControl);
	}

	void Control::OnAddToContainer()
	{
		mParent->GetEventDispatcher().InsertMouseEventListener(MouseEventId::MouseDown, std::bind(&Control::OnMouseDown, this, std::placeholders::_1));
		mParent->GetEventDispatcher().InsertMouseEventListener(MouseEventId::MouseMove, std::bind(&Control::OnMouseMove, this, std::placeholders::_1));
		mParent->GetEventDispatcher().InsertMouseEventListener(MouseEventId::MouseUp, std::bind(&Control::OnMouseUp, this, std::placeholders::_1));
		mNowState = SkinState::normal;
	}

	void Control::OnRemoveFromContainer()
	{
		mParent->GetEventDispatcher().RemoveMouseEventListener(MouseEventId::MouseDown, std::bind(&Control::OnMouseDown, this, std::placeholders::_1));
		mParent->GetEventDispatcher().RemoveMouseEventListener(MouseEventId::MouseMove, std::bind(&Control::OnMouseMove, this, std::placeholders::_1));
		mParent->GetEventDispatcher().RemoveMouseEventListener(MouseEventId::MouseUp, std::bind(&Control::OnMouseUp, this, std::placeholders::_1));
		if (mOnRemoveFromContainerLisener)
			mOnRemoveFromContainerLisener(this);
	}
	TextRenderer * Control::GetTextRenderer(FontSize fs)
	{
		return mParent->GetScene()->GetFramework()->GetUIBatches().GetTextRenderer(fs);
	}
	UIBatches & Control::GetUIBatches()
	{
		return mParent->GetScene()->GetFramework()->GetUIBatches();
	}
	void Control::DrawSkin(const XMMATRIX * matrix)
	{
		Texture * skin = nullptr;
		if (mSkin)
		{
			skin = mSkin->GetTexture(mNowState);
			if (skin)
			{
				PolygonPleTextureBinder textureBinder(GetShape()->mPolygon.mCount);
				BindingBridge bbrige;
				textureBinder.FromTexture(skin);
				bbrige.AddBinder(textureBinder);
				GetShape()->Render(*(this->mParent->GetScene()->GetFramework()->GetUIBatches().GetBatch(BATCHES_BATCH_DEFAULT_PT)), matrix, bbrige, *skin);
			}
		}
	}
}


