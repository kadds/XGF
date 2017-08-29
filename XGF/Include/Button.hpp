#pragma once
#include "Defines.hpp"
#include "Control.hpp"
#include "Texture.hpp"
#include "Rectangle.hpp"
class Button :
	public Control, public Shape::Rectangle
{
public:
	Button();
	~Button();
	void SetNormalTexture(Texture* texture) { mNormalTexture = texture; };
	void SetDowmTexture(Texture* texture) { mDowmTexture = texture; };
	void SetForceTexture(Texture* texture) { mForceTexture = texture; };
    void SetTexture(Texture* Normaltexture, Texture* Forcetexture, Texture* Dowmtexture) { mNormalTexture = Normaltexture; mForceTexture = Forcetexture; mDowmTexture = Dowmtexture; }
	virtual void OnMouseDowm(const MousePoint & mp, int c, bool isIn) override;
	virtual void OnMouseUp(const MousePoint & mp, int c, bool isIn) override;
	virtual void OnMouseMove(const MousePoint & mp, int cp, bool isIn) override;
	virtual void Render(const XMMATRIX * matrix, const Batches & batches) override;
	
protected:
	Texture* mxTexture;
	virtual Shape * GetShape() const override
	{
		return (::Shape::Rectangle *)this;
	};
	Texture* mNormalTexture;
	Texture* mForceTexture;
	Texture* mDowmTexture;
    void SetTextureState();
};

