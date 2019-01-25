#pragma once
#include "Defines.hpp"
#include "Actor.hpp"
#include <functional>
#include "ClickHelper.hpp"
#include "TextRenderer.hpp"

namespace XGF
{
	namespace Shape {
		class Rectangle;
	}

	class Event;
	class TextRenderer;
	enum class ControlState
	{
		normal,
		hover,
		active,
		disable,
	};
	/*
		控件基类
	*/
	class Control : public Actor
	{
	public:
		Control();
		Control(int id);
		virtual ~Control();
		virtual void OnMouseDown(const Event & ev);
		virtual void OnMouseUp(const Event & ev);
		virtual void OnMouseMove(const Event & ev);
		/*
		* 绑定事件，在添加到容器时调用
		*/
		virtual void OnAddToContainer() override;
		virtual void OnRemoveFromContainer() override;

		ClickHelper & GetClickHelper() { return mClickHelper; }


		TextRenderer * GetTextRenderer();
		void SetTextRenderer(TextRenderer* renderer);
		void SetOnRemoveFromContainerListener(std::function<void(Control *)> rm) { mOnRemoveFromContainerListener = rm; }

		Texture * GetSkinTexture();
	protected:
		ControlState mNowState;
		ClickHelper mClickHelper;

		std::function<void(Control *)> mOnRemoveFromContainerListener;
		static inline float minZdivision = 0.001f;
		TextRenderer * mTextRenderer;
	protected:
		//子类调用
		void DrawSkin(Shape::Rectangle & rc);
	};


}
