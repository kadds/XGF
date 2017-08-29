#include "../../Include/Polygon.hpp"
#include <math.h>
#include "../../Include/Texture.hpp"
#include "../../Include/Texture.hpp"
PolygonPle::PolygonPle(int n)
{
	mPoint = new Point[n];
    mCount = n;
}
PolygonPle::~PolygonPle()
{
    delete[] mPoint;
}

void PolygonPle::CopyTo(PolygonPle & ppe)
{
	for (int i = 0; i < mCount; i++)
	{
		ppe.mPoint[i] = mPoint[i];
	}
}

void PolygonPle::Transform(float centerXD2, float centerYD2)
{
	for (int i = 0; i < mCount; i++)
	{
		mPoint[i].x = mPoint[i].x - centerXD2;
		mPoint[i].y = -mPoint[i].y + centerYD2;
		mPoint[i].z = 0.0f;
	}
}
PolygonPle::PolygonPle(const PolygonPle & p)
{
    if (&p == this)return;
    mCount = p.mCount;
    mPoint = new Point[p.mCount];
    memcpy(mPoint, p.mPoint, sizeof(Point)*mCount);
}
void PolygonPle::TransformTo(PolygonPle * qua, float centerXD2, float centerYD2) const
{
	for (int i = 0; i < mCount; i++)
	{
		qua->mPoint[i].x = mPoint[i].x - centerXD2;
		qua->mPoint[i].y = -mPoint[i].y + centerYD2;
		qua->mPoint[i].z = 0.0f;
	}
}
int PolygonPle::GetPointCount() const
{
	return mCount;
}

void PolygonPle::MulTo(PolygonPle * pol, DirectX::CXMMATRIX matrix) const
{
	for (int i = 0; i < mCount; i++)
	{
		DirectX::XMStoreFloat3(&pol->mPoint[i], DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&mPoint[i]), matrix));
	}
}
void PolygonPle::Mul(DirectX::CXMMATRIX matrix)
{
	for (int i = 0; i < mCount; i++)
	{
		DirectX::XMStoreFloat3(&mPoint[i], DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&mPoint[i]), matrix));
	}
}
void PolygonPle::Translation(float x, float y, float z)
{
	for (int i = 0; i < mCount; i++)
	{
		mPoint[i].x += x;
		mPoint[i].y += y;
		mPoint[i].z += z;
	}
}

PolygonPleTextureBinder::PolygonPleTextureBinder(int n)
{
    mPoint = new Position[n];
    mCount = n;
}
PolygonPleTextureBinder::PolygonPleTextureBinder(const PolygonPleTextureBinder & tb)
{
    if (&tb == this) return;
    this->mCount = tb.mCount;
    mPoint = new Position[mCount];
    memcpy(this->mPoint, tb.mPoint, sizeof(Position) * mCount);
}
void PolygonPleTextureBinder::FromTexture(const Texture * tx)
{
	mPoint[0].x = mPoint[1].x = tx->GetTexturePosLeft();
	mPoint[2].x = mPoint[3].x = tx->GetTexturePosRight();
	mPoint[0].y = mPoint[3].y = tx->GetTexturePosTop();
	mPoint[1].y = mPoint[2].y = tx->GetTexturePosBottom();
}
void PolygonPleTextureBinder::CopyTo(void * Des) const
{
	memcpy(Des, mPoint, sizeof(Position) * mCount);
}
void PolygonPleTextureBinder::SetPosition(float left, float right, float top, float bottom)
{
    mPoint[0].x = mPoint[1].x = left;
    mPoint[2].x = mPoint[3].x = right;
    mPoint[0].y = mPoint[3].y = top;
    mPoint[1].y = mPoint[2].y = bottom;

}
PolygonPleTextureBinder::~PolygonPleTextureBinder()
{
    delete[] mPoint;
}
BindingBridge::BindingBridge():mCount(0)
{
}
BindingBridge::~BindingBridge()
{
  
}

void BindingBridge::AddBinder(const PolygonPleBinder & c)
{
	binders[mCount] = &c;
	mCount++;
}

const PolygonPleBinder * BindingBridge::GetBinder(int tp) const
{
	return binders[tp];
}


////////////////////////////////////////////////////////////////////////
PolygonPleColorBinder::PolygonPleColorBinder(const PolygonPleColorBinder & pcb)
{
    if (this == &pcb)
        return;
    mCount = pcb.mCount;
    mColor = new Color[mCount];
    memcpy(mColor, pcb.mColor, sizeof(Color)*mCount);
}
void PolygonPleColorBinder::Set(int start, int count, Color & c)
{
	if (count > 1)
	{
		for (int i = 0; i < count; i++)
		{
			mColor[start + i] = c;
		}
	}
	else
		mColor[start] = c;
}
Color PolygonPleColorBinder::Get(int n)
{
	return (mColor[n]);
}

///////////////////////////////////////////////////////////////////////////
PolygonPleConstantColorBinder::PolygonPleConstantColorBinder(const PolygonPleConstantColorBinder & pcb):PolygonPleColorBinder(pcb.mCount)
{
    if (this == &pcb)
        return;
    mColor[0] = pcb.mColor[0];
	mAllCount = pcb.mAllCount;
   // memcpy(mColor, pcb.mColor, sizeof(Color)*mCount);
}
void PolygonPleConstantColorBinder::CopyTo(void * Des) const
{
	for (int i = 0; i < mAllCount; i++)
	{
		*(static_cast<Color *>(Des) + i) = mColor[0];
	}
}
Color PolygonPleConstantColorBinder::Get(int n)
{
    return mColor[0];
}

PolygonPleConstantColorBinder::~PolygonPleConstantColorBinder()
{
}
PolygonPleConstantColorBinder::PolygonPleConstantColorBinder(Color & c, int ct):PolygonPleColorBinder(1)
{
    mColor[0] = c;
	mAllCount = ct;
}

/////////////////////////////////////////////////////////////////////////

void PolygonPleColorBinder::CopyTo(void * Des) const
{
	memcpy(Des, mColor, sizeof(Color) * mCount);
}
PolygonPleColorBinder::PolygonPleColorBinder(int n)
{
    mCount = n;
    mColor = new Color[n];
    //memcpy(mColor, pcb.mColor, sizeof(Color)*mCount);
}
PolygonPleColorBinder::~PolygonPleColorBinder()
{
     delete []mColor;
}


////////////////////////////////////////////////////////////////////////////
PolygonPleIndex::PolygonPleIndex(int n)
{
	mIndex = new index[n];
	mCount = n;
}
index PolygonPleIndex::Get(int n) const
{
    return mIndex[n];
}
PolygonPleIndex::~PolygonPleIndex()
{
    delete[] mIndex;
}
void PolygonPleIndex::CopyTo(void * Des, int offset) const
{
	for (int i = 0; i < mCount; i++)
	{
		*(static_cast<index *>(Des) + i) = mIndex[i] + offset;
	}
}
PolygonPleIndex::PolygonPleIndex(const PolygonPleIndex & tb)
{
    if (&tb == this) return;
    this->mCount = tb.mCount;
    mIndex = new index[mCount];
    memcpy(this->mIndex, tb.mIndex, sizeof(index) * mCount);
}

PolygonPleConstantExColorBinder::PolygonPleConstantExColorBinder(int CountInEachLayer[], int LayerCount) :PolygonPleColorBinder(LayerCount)
{
	mLayer = new int[LayerCount];
	memcpy(mLayer, CountInEachLayer, sizeof(int) * LayerCount);
	mLayerCount = LayerCount;
}

PolygonPleConstantExColorBinder::~PolygonPleConstantExColorBinder()
{
	delete[] mLayer;
}

PolygonPleConstantExColorBinder::PolygonPleConstantExColorBinder(const PolygonPleConstantExColorBinder & pcb) :PolygonPleColorBinder(pcb.mLayerCount)
{
	mLayer = new int[pcb.mLayerCount];
	memcpy(mLayer, pcb.mLayer, sizeof(int) * pcb.mLayerCount);
	mLayerCount = pcb.mLayerCount;
}

void PolygonPleConstantExColorBinder::CopyTo(void * Des) const
{
	int allPos = 0;
	for (int i = 0; i < mLayerCount; i++)
	{
		for (int j = 0; j < mLayer[i]; j++)
		{
			*(static_cast<Color *>(Des) + allPos + j) = mColor[i];
		}
		allPos += mLayer[i];
	}
}

Color PolygonPleConstantExColorBinder::Get(int n)
{
	return Color();
}

void PolygonPleConstantExColorBinder::SetLayerColor(int layer, Color & color)
{
	if (layer < mLayerCount)
	{
		mColor[layer] = color;
	}
}
