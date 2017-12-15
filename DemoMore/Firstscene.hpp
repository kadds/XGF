#pragma once
#include "../XGF/include/XGF.h"
using namespace XGF;
class FirstScene :
	public Scene, public InputListener
{
public:
	FirstScene();
	virtual ~FirstScene();
	virtual void OnCreate() override;
	virtual void OnDestory() override;
	virtual void Render(float deltaTime) override;
	virtual void Updata(float deltaTime) override;
	virtual void OnSize(int ClientX, int ClientY) override;
	virtual void OnActivate(bool isActivate) override;
	virtual SceneAnimation * OnSwitchIn();
	virtual SceneAnimation * OnSwitchOut();
	virtual void OnMouseDowm(const MousePoint &mp, int pk)  override {};
	virtual void OnMouseUp(const MousePoint &mp, int pk) override {};
	virtual void OnMouseMove(const MousePoint &mm, int pk) override;
	virtual void OnKeyDowm(Key k) override;
	virtual void OnKeyUp(Key k)  override {};
private:
	PerspectiveCamera mCamera;
	OrthoCamera mCamera2D;
	AxisRenderer mAxisRenderer;
	UILayer mUILayer;

	Batch mBatch3D;
	Shaders mShader3D;
	Shape::Cube mCube;
    Font mFont;
    TextRenderer mUITextRenderer;
    Font mFont_s;
    TextRenderer mTextRenderer_s;
	Font mFont_b;
	TextRenderer mTextRenderer_b;

	Button mBt;
	Label mLb;
	Label mLxb;
	EditText mEdit;
	EditText mEdit2;

	Texture btNormal;
	Texture btPress;
	Texture btMove;
	SceneAnimation mSceneAnimationIn;
	SceneAnimation mSceneAnimationOut;


public:
};

