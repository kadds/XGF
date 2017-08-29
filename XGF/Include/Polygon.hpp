#pragma once
#include "Defines.hpp"
#include <vector>
class Texture;

class PolygonPleBinder
{
public:
	PolygonPleBinder() {}
	virtual ~PolygonPleBinder() {}
	int mCount;
	virtual void CopyTo(void * Des) const = 0;
};

/*
索引结构
*/
class PolygonPleIndex
{
public:
	index *mIndex;
	int mCount;
	PolygonPleIndex(int n);
	~PolygonPleIndex();
	void CopyTo(void * Des, int offset) const;
    PolygonPleIndex(const PolygonPleIndex& tb);
    bool operator ==(const PolygonPleIndex &rx) const {
        if (&rx == this)
            return true;
        if (rx.mCount != mCount) return false;
        for (int i = 0; i < mCount; i++)
        {
            if (mIndex[i] != rx.mIndex[i])
                return false;
        }
        return  true;
    }
	index Get(int n) const;
};
//////////////////
/*
纹理结构
*/
class PolygonPleTextureBinder: public PolygonPleBinder
{
public:
	Position *mPoint;
    PolygonPleTextureBinder(int n);
    ~PolygonPleTextureBinder();
    PolygonPleTextureBinder(const PolygonPleTextureBinder& tb);
	void FromTexture(const Texture * tx);
    bool operator ==(const PolygonPleTextureBinder &rx) const {
        if (&rx == this)
            return true;
        if (rx.mCount != mCount) return false;
        for (int i = 0; i < mCount; i++)
        {
            if (!(mPoint[i].x == rx.mPoint[i].x && mPoint[i].y == rx.mPoint[i].y))
                return false;
        }
        return  true;
    }
	virtual void CopyTo(void * Des) const override;
    void SetPosition(float left, float right, float top, float bottom);
};

/*
颜色结构
*/
class PolygonPleColorBinder : public PolygonPleBinder
{
public:
	PolygonPleColorBinder(int n);
	~PolygonPleColorBinder();
    PolygonPleColorBinder(const PolygonPleColorBinder& pcb);
	Color * mColor;
	void Set(int start, int count, Color & c);
	virtual Color Get(int n);
	virtual void CopyTo(void * Des) const override;
    bool operator ==(const PolygonPleColorBinder &rx) const {
        if (&rx == this)
            return true;
        if (rx.mCount != mCount) return false;
        for (int i = 0; i < mCount; i++)
        {
            if (!(mColor[i].x == rx.mColor[i].x && mColor[i].y == rx.mColor[i].y && mColor[i].x == rx.mColor[i].x && mColor[i].y == rx.mColor[i].y && mColor[i].z == rx.mColor[i].z && mColor[i].w == rx.mColor[i].w))
                return false;
        }
        return  true;
    }
};
/*
恒定颜色结构
*/
class PolygonPleConstantColorBinder : public PolygonPleColorBinder
{
public:
	PolygonPleConstantColorBinder(Color & c, int Maxcount);
	~PolygonPleConstantColorBinder();
    PolygonPleConstantColorBinder(const PolygonPleConstantColorBinder& pcb);
	virtual void CopyTo(void * Des) const override;
    virtual Color Get(int n);

    bool operator ==(const PolygonPleConstantColorBinder &rx) const {
        if (&rx == this)
            return true;
        if (rx.mCount != mCount) return false;
        return  mColor == rx.mColor;
    }
	int mAllCount;
};
/*
恒定多颜色结构
*/
class PolygonPleConstantExColorBinder : public PolygonPleColorBinder
{
public:
	PolygonPleConstantExColorBinder(int CountInEachLayer[], int LayerCount);
	~PolygonPleConstantExColorBinder();
	PolygonPleConstantExColorBinder(const PolygonPleConstantExColorBinder& pcb);
	virtual void CopyTo(void * Des) const override;
	virtual Color Get(int n);
	void SetLayerColor(int layer, Color & color);
	bool operator ==(const PolygonPleConstantExColorBinder &rx) const {
		if (&rx == this)
			return true;
		if (rx.mCount != mCount) return false;
		return  mColor == rx.mColor;
	}
	int mAllCount;
	int *mLayer;
	int mLayerCount;
};

/*
顶点绑定器
*/
class BindingBridge
{
public:
	BindingBridge();
	~BindingBridge();
	int mCount;
	int GetBinderCount() const {
		return mCount;
	};
	void AddBinder(const  PolygonPleBinder & c);
	const PolygonPleBinder * GetBinder(int tp) const;
private:
	const PolygonPleBinder* binders[16];
};


/*
* 几何顶点结构
*/
class PolygonPle
{
public:
	Point *mPoint;
	int mCount;
	PolygonPle(int n);
    PolygonPle(const PolygonPle & p);
	~PolygonPle();
    bool operator ==(const PolygonPle &rx) const {
        if (&rx == this)
            return true;
        if (rx.mCount != mCount) return false;
        for (int i = 0; i < mCount; i++)
        {
            if (mPoint[i].x != rx.mPoint[i].x || mPoint[i].y != rx.mPoint[i].y || mPoint[i].z != rx.mPoint[i].z)
                return false;
        }
        return true;
    }
	void CopyTo(PolygonPle & ppe);
public:
	void Transform(float centerXD2, float centerYD2);
	void TransformTo(PolygonPle * pol,float centerXD2,float centerYD2) const;
	void Translation(float x, float y, float z);
	int GetPointCount() const;
	void Mul(DirectX::CXMMATRIX matrix);
	void MulTo(PolygonPle * pol, DirectX::CXMMATRIX matrix) const;

};
