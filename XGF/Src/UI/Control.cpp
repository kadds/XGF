#include "../../Include/Control.hpp"
#include "../../Include/Container.hpp"
namespace XGF
{
	Control::Control() :Actor()
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
	}
}


