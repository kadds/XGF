#include "../../Include/Button.hpp"
#include "../../Include/Batch.hpp"

Button::Button():mxTexture(nullptr)
{
	SetMouseEventable(true);
	SetClickable(true);
}

Button::~Button()
{
}
void Button::OnMouseDowm(const MousePoint & mp, int c, bool isIn)
{
    Actor::OnMouseDowm(mp, c, isIn);
    SetTextureState();
}
void Button::OnMouseUp(const MousePoint & mp, int c, bool isIn)
{
	Actor::OnMouseUp(mp, c, isIn);
    SetTextureState();
}
void Button::OnMouseMove(const MousePoint & mp, int cp, bool isIn)
{
    Actor::OnMouseMove(mp, cp,isIn);
    SetTextureState();
}
void Button::Render(const XMMATRIX * matrix, Batches & batches)
{
	if (mxTexture == nullptr)
		mxTexture = mNormalTexture;
	PolygonPleTextureBinder textureBinder(4);
	BindingBridge bbrige;
	textureBinder.FromTexture(mxTexture);
	bbrige.AddBinder(textureBinder);
    Shape::Render(*batches.GetBatch(BATCHES_BATCH_DEFAULT_PT), matrix, bbrige,*mxTexture);
}

void Button::SetTextureState()
{
    if (mClickHelper.GetState() == CLICK_STATE_FORCE)
        mxTexture = mForceTexture;
    if (mClickHelper.GetState() == CLICK_STATE_NORMAL)
        mxTexture = mNormalTexture;
    if (mClickHelper.GetState() == CLICK_STATE_DOWM)
        mxTexture = mDowmTexture;
}
