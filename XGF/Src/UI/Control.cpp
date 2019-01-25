#include "../../Include/Control.hpp"
#include "../../Include/Container.hpp"
#include "../../Include/Scene.hpp"
#include "../../Include/XGFramework.hpp"
namespace XGF
{
	Control::Control() :Actor()
	{
	}

	Control::Control(int id) : Actor(id)
	{
	}

	Control::~Control()
	{
	}

	void Control::OnMouseDown(const Event & ev)
	{
		bool isInControl = static_cast<Shape::Shape2D *>(GetShape())->IsInBoundBox(Point((float)ev.GetDataInt(0), (float)ev.GetDataInt(1), 0.f), GetMixMatrix());
		if (isInControl)
		{
			mNowState = ControlState::active;
		}
		else
		{
			mNowState = ControlState::normal;
		}
		mClickHelper.OnMouseDown({ ev.GetDataInt(0), ev.GetDataInt(1) }, isInControl);
	}

	void Control::OnMouseUp(const Event & ev)
	{
		bool isInControl = static_cast<Shape::Shape2D *>(GetShape())->IsInBoundBox(Point((float)ev.GetDataInt(0), (float)ev.GetDataInt(1), 0.f), GetMixMatrix());
		if (isInControl)
		{
			mNowState = ControlState::hover;
		}
		mClickHelper.OnMouseUp(this, { ev.GetDataInt(0), ev.GetDataInt(1) }, isInControl);
	}

	void Control::OnMouseMove(const Event & ev)
	{
		bool isInControl = static_cast<Shape::Shape2D *>(GetShape())->IsInBoundBox(Point((float)ev.GetDataInt(0), (float)ev.GetDataInt(1), 0.f), GetMixMatrix());
		if (isInControl)
		{
			if(mNowState != ControlState::active)
				mNowState = ControlState::hover;
		}
		else
		{
			mNowState = ControlState::normal;
		}
		mClickHelper.OnMouseMove(isInControl);
	}

	void Control::OnAddToContainer()
	{
		mParent->GetEventDispatcher().InsertMouseEventListener(MouseEventId::MouseDown, std::bind(&Control::OnMouseDown, this, std::placeholders::_1));
		mParent->GetEventDispatcher().InsertMouseEventListener(MouseEventId::MouseMove, std::bind(&Control::OnMouseMove, this, std::placeholders::_1));
		mParent->GetEventDispatcher().InsertMouseEventListener(MouseEventId::MouseUp, std::bind(&Control::OnMouseUp, this, std::placeholders::_1));
		mNowState = ControlState::normal;
	}

	void Control::OnRemoveFromContainer()
	{
		mParent->GetEventDispatcher().RemoveMouseEventListener(MouseEventId::MouseDown, std::bind(&Control::OnMouseDown, this, std::placeholders::_1));
		mParent->GetEventDispatcher().RemoveMouseEventListener(MouseEventId::MouseMove, std::bind(&Control::OnMouseMove, this, std::placeholders::_1));
		mParent->GetEventDispatcher().RemoveMouseEventListener(MouseEventId::MouseUp, std::bind(&Control::OnMouseUp, this, std::placeholders::_1));
		if (mOnRemoveFromContainerListener)
			mOnRemoveFromContainerListener(this);
	}
	TextRenderer * Control::GetTextRenderer()
	{
		return mTextRenderer;
	}

	void Control::SetTextRenderer(TextRenderer* renderer)
	{
		mTextRenderer = renderer;
	}

	Texture* Control::GetSkinTexture()
	{
		Texture * skin = nullptr;
		
		return nullptr;
	}

	void Control::DrawSkin(Shape::Rectangle & rc)
	{

	}
}


