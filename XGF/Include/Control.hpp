#pragma once
#include "Defines.hpp"
#include "Actor.hpp"
#include "Skin.hpp"
#include <math.h>
#include "UIBatches.hpp"
#include <functional>
#include "ClickHelper.hpp"

namespace XGF
{
	class Event;
	class TextRenderer;
	/*
		控件基类
	*/
	class Control : public Actor
	{
	public:
		Control();
		Control(int id);
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


		virtual TextRenderer * GetTextRenderer(FontSize fs);
		UIBatches & GetUIBatches();

		void SetOnRemoveFromContainerListener(std::function<void(Control *)> rm) { mOnRemoveFromContainerListener = rm; }
	protected:
		std::shared_ptr<Skin> mSkin;
		SkinState mNowState;
		ClickHelper mClickHelper;
		FontSize mFontSize;

		std::function<void(Control *)> mOnRemoveFromContainerListener;
		static inline float minZdivision = 0.001f;
	protected:
		//子类调用
		void DrawSkin();
	};


}
