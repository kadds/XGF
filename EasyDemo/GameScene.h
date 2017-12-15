#pragma once
#include "..\XGF\Include\XGF.h"
using namespace XGF;
class GameScene :
	public Scene
{
public:
	GameScene();
	virtual ~GameScene();
	void MyRender(WVPMatrix & wvp, bool);
	virtual void OnCreate() override;
	virtual void OnDestory() override;
	virtual void Render(float deltaTime) override;
	virtual void Updata(float deltaTime) override;
	virtual void OnSize(int ClientX, int ClientY) override;
	virtual void OnActivate(bool isActivate) override;

private:
	ShapeRenderer mShapeRenderer;
	OrthoCamera mCamera;
	RenderToTexture mRenderToTexture;

	Font mFont;
	TextRenderer mTextRenderer;

	Batch mTextureBatch;

	Shape::Rectangle mRc;

	Texture mTexture;

	float mTime;
};

