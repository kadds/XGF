#pragma once
#include "Defines.hpp"
#include "Layer.hpp"
#include <vector>
#include "Control.hpp"
#include "InputListener.hpp"
class UIBatches;
class TextRenderer;
/*
UI绘制层
所有的 UI 控件都要在该层下 
为了接受InputEvent，请把此类的对象添加到InputListene，如 Framework->AddInputListene
*/
class UILayer :
	public Layer,public InputListener
{
public:
	friend class Actor;
	UILayer();
	~UILayer();
    
    virtual void OnCreate();
	virtual void OnDestory();
	virtual void Render();
    virtual void Updata(float deltaTime);
	virtual void OnSize(int ClientX, int ClientY);
	virtual void OnActivate(bool a) {}

	virtual void OnMouseDowm(const MousePoint &mp, int pk);
	virtual void OnMouseUp(const MousePoint &mp, int pk);
	virtual void OnMouseMove(const MousePoint &mm, int pk);
	virtual void OnKeyDowm(Key k);
	virtual void OnKeyUp(Key k);

	virtual void Add(const Actor * actor);
	UIBatches * GetUIBatches() { return (UIBatches*)mBatches; };
protected:
	float mZdeep;
private:
};

