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
	��Ա��
	Tip: �̳�ʱ����д Render ��GetShape ����������Button��Lable��EditText��
	*/
	class Actor : public GObject
	{
	public:
		Actor();
		Actor(int id);
		virtual ~Actor();
		//��ܵ���
		void _Render();
		//��ܵ���
		void _Update(float deltaTime);
		virtual void Update(float deltaTime) {};
		virtual void Render(const XMMATRIX * matrix) = 0;
		
		Transform & GetTransform() { return mTransform; };

		//���� ��this��static_cast<YouShapeClass>
		virtual Shape::Shape *GetShape() const = 0;
		void SetParent(Container * container) { mParent = container; };

		virtual void OnAddToContainer() = 0;
		virtual void OnRemoveFromContainer() = 0;

		int GetId() { return mId; };
		void SetId(int id) { mId = id; };
	protected:
		Container * mParent;
		Transform mTransform;
		int mId;
	};



}
