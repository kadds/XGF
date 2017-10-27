#pragma once
#include "EasyDemo.h"
class TFramework :
	public XGFramework
{
public:
	virtual void OnCreate() override;;
	virtual void OnDestory() override;;
	virtual void OnSize(int ClientX, int ClientY) override;;
	virtual void OnMessage(const Event& ev) override;
	virtual void OnActivate(bool isActivate)override;;
	virtual bool OnClose()override;;
	virtual void Render() override;;
	virtual void Update(float time)override;;
	TFramework();
	~TFramework();

	void MyRender(WVPMatrix & wvp, bool );
private:
	ShapeRenderer mShapeRenderer;
	OrthoCamera mCamera;
	RenderToTexture mRenderToTexture;

	Font mFont;
	TextRenderer mTextRenderer;

	Batch mTextureBatch;
	Shader mTextureShader;

	Shape::Rectangle mRc;

	Texture mTexture;
};

