#pragma once
#include "../XGF/include/XGF.h"
using namespace XGF;
class SecondScene :
	public Scene, public InputListener
{
public:
	SecondScene();
	~SecondScene();

	virtual void OnCreate() override;
	virtual void OnDestory() override;
	virtual void Render(float deltaTime) override;
	virtual void Updata(float deltaTime) override;
	virtual void OnSize(int ClientX, int ClientY) override;
	virtual void OnActivate(bool isActivate) override;

	virtual void OnMouseDowm(const MousePoint &mp, int pk)  override {};
	virtual void OnMouseUp(const MousePoint &mp, int pk) override {};
	virtual void OnMouseMove(const MousePoint &mm, int pk) override;
	virtual void OnKeyDowm(Key k) override;
	virtual void OnKeyUp(Key k)  override {};
	virtual SceneAnimation * OnSwitchIn() { return &mSceneAnimationIn; };
private:
	GridRenderer mGridRender;
	Font mFont;
	TextRenderer mTextRenderer;
	TextRenderer mUITextRenderer;
	Animation mAnimation;
	Texture mCursorTexture;
	AxisRenderer mAxisRenderer;


	OrthoCamera mCamera;
	FPSCamera mCamera3D;
	UILayer mUILayer;

	Lable mLable;
	Lable mLable2;
	SceneAnimation mSceneAnimationIn;
};

