#include "../../Include/Polygon.hpp"
#include <math.h>
#include "../../Include/Texture.hpp"

namespace XGF
{
	BindingBridge::BindingBridge()
	{
	}
	BindingBridge::~BindingBridge()
	{

	}

	BindingBridge::BindingBridge(const BindingBridge & c)
	{
		binders = c.binders;
	}

	void BindingBridge::SetBinder(const std::shared_ptr<PolygonPleBinder> c, int pos)
	{
		XGF_ASSERT(pos < binders.size());
		XGF_ASSERT(pos >= 0);
		binders[pos] = c;
	}

	void BindingBridge::InsertBinder(const std::shared_ptr<PolygonPleBinder> c, int pos)
	{
		XGF_ASSERT(pos < binders.size());
		XGF_ASSERT(pos >= 0);
		binders.insert( binders.begin() + pos ,c);
	}

	void BindingBridge::InsertBinder(const BindingBridge & c, int pos)
	{
		XGF_ASSERT(pos < binders.size());
		XGF_ASSERT(pos >= 0);
		binders.insert(binders.begin() + pos, c.binders.begin(), c.binders.end());
	}

	void BindingBridge::AddBinder(const std::shared_ptr<PolygonPleBinder> c)
	{
		binders.push_back(c);
	}

	void BindingBridge::AddBinder(const BindingBridge & c)
	{
		binders.insert(binders.end(), c.binders.begin(), c.binders.end());
	}

	void BindingBridge::AddBinder(const std::vector<std::shared_ptr<PolygonPleBinder>>& c)
	{
		binders.insert(binders.end(), c.begin(), c.end());
	}

	void BindingBridge::AddPlaceHolder()
	{
		binders.push_back(nullptr);
	}

	void BindingBridge::RemoveBinder(int index)
	{
		XGF_ASSERT(index < binders.size());
		binders.erase(binders.begin() + index);
	}

	void BindingBridge::RemoveFrom(int indexStart)
	{
		XGF_ASSERT(indexStart < binders.size());
		XGF_ASSERT(indexStart >= 0);
		binders.erase(binders.begin() + indexStart, binders.end());
	}

	void BindingBridge::Clear()
	{
		binders.clear();
	}

	////////////////////////////////////////////////////////////////////////////
	PolygonPleIndex::PolygonPleIndex(int n)
	{
		mIndex = new Index[n];
		mCount = n;
	}
	Index PolygonPleIndex::Get(int n) const
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
			*(static_cast<Index *>(Des) + i) = mIndex[i] + offset;
		}
	}
	PolygonPleIndex::PolygonPleIndex(const PolygonPleIndex & tb)
	{
		if (&tb == this) return;
		this->mCount = tb.mCount;
		mIndex = new Index[mCount];
		memcpy(this->mIndex, tb.mIndex, sizeof(Index) * mCount);
	}

	void PolygonPleIndex::ShrinkTo(int count)
	{
		XGF_ASSERT(mCount >= count);
		if(mCount >= count)
		{
			mCount = count;
		}
	}
}
