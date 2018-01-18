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
		virtual ~Actor();
		//��ܵ���
		void _Render();
		//��ܵ���
		void _Updata(float deltaTime);

		virtual void Render(const XMMATRIX * matrix) = 0;
		
		Transform & GetTransform() { return mTransform; };

		//���� ��this��static_cast<YouShapeClass>
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
