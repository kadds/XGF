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
		bool isInControl = static_cast<Shape::Shape2D *>(GetShape())->IsInBoundBox(Point((float)ev.GetDataInt(0), (float)ev.GetDataInt(1), 0.f), GetMixMatrix());
		if (isInControl)
		{
			mNowState = SkinState::active;
		}
		else
		{
			mNowState = SkinState::normal;
		}
		mClickHelper.OnMouseDown({ ev.GetDataInt(0), ev.GetDataInt(1) }, isInControl);
	}

	void Control::OnMouseUp(const Event & ev)
	{
		bool isInControl = static_cast<Shape::Shape2D *>(GetShape())->IsInBoundBox(Point((float)ev.GetDataInt(0), (float)ev.GetDataInt(1), 0.f), GetMixMatrix());
		if (isInControl)
		{
			mNowState = SkinState::hover;
		}
		mClickHelper.OnMouseUp(this, { ev.GetDataInt(0), ev.GetDataInt(1) }, isInControl);
	}

	void Control::OnMouseMove(const Event & ev)
	{
		bool isInControl = static_cast<Shape::Shape2D *>(GetShape())->IsInBoundBox(Point((float)ev.GetDataInt(0), (float)ev.GetDataInt(1), 0.f), GetMixMatrix());
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
		if (mOnRemoveFromContainerListener)
			mOnRemoveFromContainerListener(this);
	}
	TextRenderer * Control::GetTextRenderer(FontSize fs)
	{
		return mParent->GetScene().GetFramework().GetUIBatches().GetTextRenderer(fs);
	}
	UIBatches & Control::GetUIBatches()
	{
		return mParent->GetScene().GetFramework().GetUIBatches();
	}

	Texture* Control::GetSkinTexture()
	{
		Texture * skin = nullptr;
		if (mSkin)
		{
			skin = mSkin->GetTexture(mNowState);
			return skin;
		}
		return nullptr;
	}

	void Control::DrawSkin(Shape::Rectangle & rc)
	{
		Texture * skin = nullptr;
		if (mSkin)
		{
			skin = mSkin->GetTexture(mNowState);
			if (skin)
			{
				auto shape = GetShape();
				auto textureBinder = std::make_shared<PolygonPleTextureBinder>(4);
				BindingBridge bbrige;
				
				bbrige.AddBinder(rc.mPolygon);
				textureBinder->GetData(1).x = textureBinder->GetData(0).x = 0.f;
				textureBinder->GetData(2).x = textureBinder->GetData(3).x = 1.f;
				textureBinder->GetData(1).y = textureBinder->GetData(2).y = 1.f;
				textureBinder->GetData(3).y = textureBinder->GetData(0).y = 0.f;

				bbrige.AddBinder(textureBinder);
				mParent->GetScene().GetFramework().GetUIBatches().GetBatch(BATCHES_BATCH_DEFAULT_PT)->GetShaderStage()->SetPSSRV(0, skin->GetRawTexture());
				mParent->GetScene().GetFramework().GetUIBatches().GetBatch(BATCHES_BATCH_DEFAULT_PT)->DrawPolygon(rc.mPolygonPleIndex, bbrige);
			}
		}
	}
}


