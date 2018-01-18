#pragma once
#include "Defines.hpp"
#include "Polygon.hpp"
#include "ClickHelper.hpp"
#include "Shape.hpp"
#include <memory>
#include "GObject.hpp"
#include "Transform.hpp"
namespace XGF
{
	class Container;
	/*
	演员类
	Tip: 继承时请重写 Render ，GetShape 函数。参照Button，Lable，EditText类
	*/
	class Actor : public GObject
	{
	public:
		Actor();
		virtual ~Actor();
		//框架调用
		void _Render();
		//框架调用
		void _Updata(float deltaTime);

		virtual void Render(const XMMATRIX * matrix) = 0;
		
		Transform & GetTransform() { return mTransform; };

		//返回 对this做static_cast<YouShapeClass>
		virtual Shape::Shape *GetShape() const = 0;
		void SetParent(Container * container) { mParent = container; };

		virtual void OnAddToContainer() = 0;
		virtual void OnRemoveFromContainer() = 0;
	protected:
		
		Container * mParent;
		Transform mTransform;
		int mId;
	};



}
