#pragma once
#include "Defines.hpp"
#include "Shape.hpp"
namespace XGF
{
	class Container;
	/*
	演员类
	Tip: 继承时请重写 Render ，GetShape 函数。参照Button，Label，EditText类
	*/
	class Actor
	{
	public:
		Actor();
		Actor(int id);
		virtual ~Actor();
		//框架调用
		void _Render();
		//框架调用
		void _Update(float deltaTime);
		virtual void Update(float deltaTime) {};
		virtual void Render() = 0;

		//返回 对this做static_cast<YouShapeClass>
		virtual Shape::Shape *GetShape() const = 0;
		void SetParent(Container * container) { mParent = container; };

		virtual void OnAddToContainer() = 0;
		virtual void OnRemoveFromContainer() = 0;

		int GetId() const { return mId; }
		void SetId(int id) { mId = id; };
	protected:
		Container * mParent;
		
		int mId;
	};



}
