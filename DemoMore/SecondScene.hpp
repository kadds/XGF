#pragma once
#include "../XGF/include/XGF.h"
class SecondScene :
	public Scene
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
private:
	FPS mFPS;
	Font mFont;
	TextRenderer mTextRenderer;

	Batches mBatches;
	UIBatches mUIBatches;

	OrthoCamera mCamera;
	UILayer mUILayer;

	Lable mLable;
	Lable mLable2;

};

