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
		Control basic class
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
		* Called when added to a container
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

		void DrawSkin(Shape::Rectangle & rc);
	};


}
