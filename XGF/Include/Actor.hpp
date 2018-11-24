#pragma once
#include "Defines.hpp"
#include "Shape.hpp"
namespace XGF
{
	class Container;
	/*
	��Ա��
	Tip: �̳�ʱ����д Render ��GetShape ����������Button��Label��EditText��
	*/
	class Actor
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
		virtual void Render() = 0;

		//���� ��this��static_cast<YouShapeClass>
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
