#pragma once
#include "Defines.hpp"
#include "Polygon.hpp"
#include "InputListener.hpp"
#include "ClickHelper.hpp"
#include "ActionInterface.hpp"
#include "Actions.hpp"
#include "Shape.hpp"
#include "Batches.hpp"
#include <memory>
#include "Action.hpp"

class Actor;
class Layer;
class Batch;

/*
��Ա��
Ĭ��Ϊ3D��Ա
2D��Ա�뿴Control ���ʵ��
Tip: �̳�ʱ����д Render ��GetShape ����������Button��Lable��EditText��
*/
class Actor : public ActionInterface
{
public:
	Actor();
	virtual ~Actor();
	//��ܵ���
    void _Render(const Batches & bs);
	//��ܵ���
    void _Updata(float deltaTime);

	virtual void Render(const XMMATRIX * matrix, const Batches & batches) = 0;
	
	//��ܵ���
	//����û�����
    bool IsInBBox(Point p);

    Point GetOrigin() const;
    Point GetAngle() const;
    Point GetScale() const;

    void SetAction(std::unique_ptr<Action> action);
	void StartAction ();
    void StopAction();
    void SetRotationAngle(Point angle);
    void SetScale(Point p);
    void SetTranslation(Point translation);
	//��ܵ���
	//����û�����
    bool CanClick() const{ return mClickable; }
	//��ܵ���
	//����û�����
    bool CanGetMouseEvent() const { return mCanGetMouseEvent; }
    void SetClickable(bool b) { mClickable = b; }
    void SetMouseEventable(bool b) { mCanGetMouseEvent = b; }

    virtual void OnMouseDowm(const MousePoint & mp, int c, bool isIn);
    virtual void OnMouseUp(const MousePoint & mp, int c, bool isIn);
    virtual void OnMouseMove(const MousePoint & mp, int cp, bool isIn);

    void AddOnClickListener(OnClickListener l) { mClickHelper.AddOnClickListener(l); }
    void SetMouseDowmListener(OnMouseDowmListener l) { onMouseDowmListener = l; }
    void SetMouseMoveListener(OnMouseMoveListener l) { onMouseMoveListener = l; }
    void SetMouseUpListener(OnMouseUpListener l) { onMouseUpListener = l; }
    void RefreshMatrix();
    void ChangeState();
	void SetScaleOrigin(Point &p) { mScaleOrigin = p; }
	void SetRotationOrigin(Point &p) { mRotationOrigin = p; }
	void OnActionCompleted();
	
	//ActionInterface��ܵ���
	virtual void OnPositionChange(const Point & p, int ID);
	//ActionInterface��ܵ���
	virtual void OnAngleChange(const Point & p, int ID);
	//ActionInterface��ܵ���
	virtual void OnScaleChange(const Point & p, int ID);
	//ActionInterface��ܵ���
	virtual void OnColorChange(const Color & p, int ID);
	//ActionInterface��ܵ���
	virtual void OnAlphaChange(float a, int ID);
	//ActionInterface��ܵ���
	virtual void GetPosition(Point & p, int ID) const;
	//ActionInterface��ܵ���
	virtual void GetAngle(Point & p, int ID) const;
	//ActionInterface��ܵ���
	virtual void GetScale(Point & p, int ID) const;
	//ActionInterface��ܵ���
	virtual void GetColor(Color & p, int ID) const;
	//ActionInterface��ܵ���
	virtual void GetAlpha(float & p, int ID) const;

	//���� ��this��static_cast<YouShapeClass>
	virtual Shape::Shape *GetShape() const= 0;
protected:
    virtual XMMATRIX CalculateMatrix()  const;
    Point mRotationOrigin;
	Point mScaleOrigin;
    Point mRotation;
    Point mScale;
    Point mTranslation;
	
	Color mColor;

    bool mIsChange;
    bool mClickable;
    bool mCanGetMouseEvent;
	Actions mActions;

    DirectX::XMFLOAT4X4 mMatrix;

	//����������
    OnMouseDowmListener onMouseDowmListener;
    OnMouseUpListener onMouseUpListener;
    OnMouseMoveListener onMouseMoveListener;
    ClickHelper mClickHelper;
};


