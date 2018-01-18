#pragma once
#include "Defines.hpp"
#include "Actor.hpp"
#include "Skin.hpp"
#include <math.h>
namespace XGF
{
	class Control : public Actor
	{
	public:
		Control();
		virtual ~Control();
		std::shared_ptr<Skin> SetSkin(std::shared_ptr<Skin>);
		virtual void OnMouseDown(const Event & ev);
		virtual void OnMouseUp(const Event & ev);
		virtual void OnMouseMove(const Event & ev);
		/*
		* 绑定事件，在添加到容器时调用
		*/
		virtual void OnAddToContainer() override;
		virtual void OnRemoveFromContainer() override;

		ClickHelper & GetClickHelper() { return mClickHelper; }
	protected:
		std::shared_ptr<Skin> mSkin;
		SkinState mNowState;
		ClickHelper mClickHelper;
	};


}
