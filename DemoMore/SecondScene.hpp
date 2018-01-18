#pragma once
#include "../XGF/include/XGF.h"
using namespace XGF;
class SecondScene :
	public Scene
{
public:
	SecondScene();
	~SecondScene();

	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Render(float deltaTime) override;
	virtual void Updata(float deltaTime) override;
	virtual void OnSize(int ClientX, int ClientY) override;

	virtual void OnActivate(bool isActivate) override;

	void OnMouseMove(const Event & ev);
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

	Label mLabel;
	Label mLabel2;
};

