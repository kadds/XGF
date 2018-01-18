#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "Texture.hpp"
#include "Rectangle.hpp"
namespace XGF
{
	class Button :
		public Control, public Shape::Rectangle
	{
	public:
		Button();
		~Button();

		virtual void Render(const XMMATRIX * matrix) override;

	protected:
		virtual Shape * GetShape() const override
		{
			return (::XGF::Shape::Rectangle *)this;
		};

	};
}


