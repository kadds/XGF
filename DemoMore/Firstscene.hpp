#pragma once
#include "../XGF/include/XGF.h"
using namespace XGF;
class FirstScene :
	public Scene
{
public:
	FirstScene();
	virtual ~FirstScene();
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Render(float deltaTime) override;
	virtual void Update(float deltaTime) override;
	virtual void OnSize(int ClientX, int ClientY) override;

private:
	PerspectiveCamera mCamera;
	OrthoCamera mCamera2D;
	AxisRenderer mAxisRenderer;

	Batch mBatch3D;
	Shaders mShader3D;
	Shape::Cube mCube;
    Font mFont;
    TextRenderer mUITextRenderer;
    Font mFont_s;
    TextRenderer mTextRenderer_s;
	Font mFont_b;
	TextRenderer mTextRenderer_b;

	TextureResourceManager mTextureResourceManager;
public:
};

