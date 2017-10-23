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
演员类
默认为3D演员
2D演员请看Control 类的实现
Tip: 继承时请重写 Render ，GetShape 函数。参照Button，Lable，EditText类
*/
class Actor : public ActionInterface
{
public:
	Actor();
	virtual ~Actor();
	//框架调用
    void _Render(const Batches & bs);
	//框架调用
    void _Updata(float deltaTime);

	virtual void Render(const XMMATRIX * matrix, const Batches & batches) = 0;
	
	//框架调用
	//亦可用户调用
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
	//框架调用
	//亦可用户调用
    bool CanClick() const{ return mClickable; }
	//框架调用
	//亦可用户调用
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
	
	//ActionInterface框架调用
	virtual void OnPositionChange(const Point & p, int ID);
	//ActionInterface框架调用
	virtual void OnAngleChange(const Point & p, int ID);
	//ActionInterface框架调用
	virtual void OnScaleChange(const Point & p, int ID);
	//ActionInterface框架调用
	virtual void OnColorChange(const Color & p, int ID);
	//ActionInterface框架调用
	virtual void OnAlphaChange(float a, int ID);
	//ActionInterface框架调用
	virtual void GetPosition(Point & p, int ID) const;
	//ActionInterface框架调用
	virtual void GetAngle(Point & p, int ID) const;
	//ActionInterface框架调用
	virtual void GetScale(Point & p, int ID) const;
	//ActionInterface框架调用
	virtual void GetColor(Color & p, int ID) const;
	//ActionInterface框架调用
	virtual void GetAlpha(float & p, int ID) const;

	//返回 对this做static_cast<YouShapeClass>
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

	//监听器变量
    OnMouseDowmListener onMouseDowmListener;
    OnMouseUpListener onMouseUpListener;
    OnMouseMoveListener onMouseMoveListener;
    ClickHelper mClickHelper;
};


