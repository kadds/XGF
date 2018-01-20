#include "../../Include/Polygon.hpp"
#include <math.h>
#include "../../Include/Texture.hpp"
#include "../../Include/Texture.hpp"
namespace XGF
{
	PolygonPlePoint3::PolygonPlePoint3(int n)
	{
		mPoint = new Point[n];
		mCount = n;
	}
	PolygonPlePoint3::~PolygonPlePoint3()
	{
		delete[] mPoint;
	}

	void PolygonPlePoint3::CopyTo(void * Des, unsigned int chunk) const
	{
		char * des = (char *)Des;
		for (int i = 0; i < mCount; i++)
		{
			*((Point *)des) = mPoint[i];
			des += chunk;
		}
	}

	void PolygonPlePoint3::CopyTo(PolygonPlePoint3 & ppe)
	{
		for (int i = 0; i < mCount; i++)
		{
			ppe.mPoint[i] = mPoint[i];
		}
	}

	void PolygonPlePoint3::Transform(float centerXD2, float centerYD2)
	{
		for (int i = 0; i < mCount; i++)
		{
			mPoint[i].x = mPoint[i].x - centerXD2;
			mPoint[i].y = -mPoint[i].y + centerYD2;
			mPoint[i].z = 0.0f;
		}
	}
	PolygonPlePoint3::PolygonPlePoint3(const PolygonPlePoint3 & p)
	{
		if (&p == this)return;
		mCount = p.mCount;
		mPoint = new Point[p.mCount];
		memcpy(mPoint, p.mPoint, sizeof(Point)*mCount);
	}
	void PolygonPlePoint3::TransformTo(PolygonPlePoint3 * qua, float centerXD2, float centerYD2) const
	{
		for (int i = 0; i < mCount; i++)
		{
			qua->mPoint[i].x = mPoint[i].x - centerXD2;
			qua->mPoint[i].y = -mPoint[i].y + centerYD2;
			qua->mPoint[i].z = 0.0f;
		}
	}

	void PolygonPlePoint3::MulTo(PolygonPlePoint3 * pol, DirectX::CXMMATRIX matrix) const
	{
		for (int i = 0; i < mCount; i++)
		{
			DirectX::XMStoreFloat3(&pol->mPoint[i], DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&mPoint[i]), matrix));
		}
	}
	void PolygonPlePoint3::Mul(DirectX::CXMMATRIX matrix)
	{
		for (int i = 0; i < mCount; i++)
		{
			DirectX::XMStoreFloat3(&mPoint[i], DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&mPoint[i]), matrix));
		}
	}
	void PolygonPlePoint3::Translation(float x, float y, float z)
	{
		for (int i = 0; i < mCount; i++)
		{
			mPoint[i].x += x;
			mPoint[i].y += y;
			mPoint[i].z += z;
		}
	}



	PolygonPlePoint4::PolygonPlePoint4(int n)
	{
		mPoint = new Point4[n];
		mCount = n;
	}
	PolygonPlePoint4::~PolygonPlePoint4()
	{
		delete[] mPoint;
	}

	void PolygonPlePoint4::CopyTo(void * Des, unsigned int chunk) const
	{
		char * des = (char *)Des;
		for (int i = 0; i < mCount; i++)
		{
			*((Point4 *)des) = mPoint[i];
			des += chunk;
		}
	}

	void PolygonPlePoint4::CopyTo(PolygonPlePoint4 & ppe)
	{
		for (int i = 0; i < mCount; i++)
		{
			ppe.mPoint[i] = mPoint[i];
		}
	}

	void PolygonPlePoint4::Transform(float centerXD2, float centerYD2)
	{
		for (int i = 0; i < mCount; i++)
		{
			mPoint[i].x = mPoint[i].x - centerXD2;
			mPoint[i].y = -mPoint[i].y + centerYD2;
			mPoint[i].z = 0.0f;
		}
	}
	PolygonPlePoint4::PolygonPlePoint4(const PolygonPlePoint4 & p)
	{
		if (&p == this)return;
		mCount = p.mCount;
		mPoint = new Point4[p.mCount];
		memcpy(mPoint, p.mPoint, sizeof(Point)*mCount);
	}
	void PolygonPlePoint4::TransformTo(PolygonPlePoint4 * qua, float centerXD2, float centerYD2) const
	{
		for (int i = 0; i < mCount; i++)
		{
			qua->mPoint[i].x = mPoint[i].x - centerXD2;
			qua->mPoint[i].y = -mPoint[i].y + centerYD2;
			qua->mPoint[i].z = 0.0f;
		}
	}

	void PolygonPlePoint4::MulTo(PolygonPlePoint4 * pol, DirectX::CXMMATRIX matrix) const
	{
		for (int i = 0; i < mCount; i++)
		{
			DirectX::XMStoreFloat4(&pol->mPoint[i], DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&mPoint[i]), matrix));
		}
	}
	void PolygonPlePoint4::Mul(DirectX::CXMMATRIX matrix)
	{
		for (int i = 0; i < mCount; i++)
		{
			DirectX::XMStoreFloat4(&mPoint[i], DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&mPoint[i]), matrix));
		}
	}
	void PolygonPlePoint4::Translation(float x, float y, float z)
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
		mPoint[0].x = mPoint[1].x = tx->GetLeft();
		mPoint[2].x = mPoint[3].x = tx->GetRight();
		mPoint[0].y = mPoint[3].y = tx->GetTop();
		mPoint[1].y = mPoint[2].y = tx->GetBottom();
		if (tx->Is9Path() && this->mCount >= 8)
		{
			mPoint[4].x = mPoint[5].x = tx->Get9PathLeft();
			mPoint[6].x = mPoint[7].x = tx->Get9PathRight();
			mPoint[4].y = mPoint[7].y = tx->Get9PathTop();
			mPoint[5].y = mPoint[6].y = tx->Get9PathBottom();
		}
	}
	void PolygonPleTextureBinder::CopyTo(void * Des, unsigned int chunk) const
	{
		char * c = static_cast<char *>(Des);
		for (int i = 0; i < mCount; i++)
		{
			*(Position *)c = mPoint[i];
			c += chunk;
		}
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
	BindingBridge::BindingBridge() :mCount(0)
	{
	}
	BindingBridge::~BindingBridge()
	{

	}

	void BindingBridge::SetBinder(const PolygonPleBinder & c, int pos)
	{
		binders[pos] = &c;
	}

	void BindingBridge::InsertBinder(const PolygonPleBinder & c, int pos)
	{
		for (int i = mCount; i > pos; i--)
		{
			binders[i] = binders[i - 1];//TODO:: some problem!!
		}
		binders[pos] = &c;
		mCount++;
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
	PolygonPleConstantColorBinder::PolygonPleConstantColorBinder(const PolygonPleConstantColorBinder & pcb) :PolygonPleColorBinder(pcb.mCount)
	{
		if (this == &pcb)
			return;
		mColor[0] = pcb.mColor[0];
		mAllCount = pcb.mAllCount;
		// memcpy(mColor, pcb.mColor, sizeof(Color)*mCount);
	}
	void PolygonPleConstantColorBinder::CopyTo(void * Des, unsigned int chunk) const
	{
		char * c = static_cast<char *>(Des);
		for (int i = 0; i < mAllCount; i++)
		{
			*(Color *)c = mColor[0];
			c += chunk;
		}
	}
	Color PolygonPleConstantColorBinder::Get(int n)
	{
		return mColor[0];
	}

	PolygonPleConstantColorBinder::~PolygonPleConstantColorBinder()
	{
	}
	PolygonPleConstantColorBinder::PolygonPleConstantColorBinder(Color & c, int ct) :PolygonPleColorBinder(1)
	{
		mColor[0] = c;
		mAllCount = ct;
	}

	/////////////////////////////////////////////////////////////////////////

	void PolygonPleColorBinder::CopyTo(void * Des, unsigned int chunk) const
	{
		char * c = static_cast<char *>(Des);
		for (int i = 0; i < mCount; i++)
		{
			*(Color *)c = mColor[i];
			c += chunk;
		}
	}
	PolygonPleColorBinder::PolygonPleColorBinder(int n)
	{
		mCount = n;
		mColor = new Color[n];
		//memcpy(mColor, pcb.mColor, sizeof(Color)*mCount);
	}
	PolygonPleColorBinder::~PolygonPleColorBinder()
	{
		delete[]mColor;
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

	void PolygonPleConstantExColorBinder::CopyTo(void * Des, unsigned int chunk) const
	{
		int allPos = 0;
		char * c = static_cast<char *>(Des);
		for (int i = 0; i < mLayerCount; i++)
		{
			for (int j = 0; j < mLayer[i]; j++)
			{
				*((Color *)c) = mColor[i];
				c += chunk;
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


	PolygonPleDataBinder::PolygonPleDataBinder()
	{
		mCount = 1;
	}

	PolygonPleDataBinder::~PolygonPleDataBinder()
	{
	}

	PolygonPleDataBinder::PolygonPleDataBinder(const PolygonPleDataBinder & pcb)
	{
		mData = pcb.mData;
	}

	void PolygonPleDataBinder::CopyTo(void * Des, unsigned int chunk) const
	{
		*(static_cast<XMFLOAT4 *>(Des)) = mData;
	}

}
