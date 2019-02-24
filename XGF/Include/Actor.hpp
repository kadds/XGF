#pragma once
#include "Defines.hpp"
#include "Shape.hpp"
namespace XGF
{
	class Container;
	/*
	Tip: Rewrite the Render GetShape virtual functions when inheriting. Refer to Button, Label, EditText class
	*/
	class Actor
	{
	public:
		Actor();
		Actor(int id);
		virtual ~Actor();

		void _Render();

		void _Update(float deltaTime);
		virtual void Update(float deltaTime) {};
		virtual void Render() = 0;

		// must be return static_cast<YouShapeClass>(this)
		virtual Shape::Shape *GetShape() const = 0;
		void SetParent(Container * container) { mParent = container; };

		virtual void OnAddToContainer() = 0;
		virtual void OnRemoveFromContainer() = 0;

		int GetId() const { return mId; }
		void SetId(int id) { mId = id; };
	protected:
		const SM::Matrix & GetMixMatrix();
		Container * mParent;
	private:
		SM::Matrix mMatrix;
		int mId;
	};



}
