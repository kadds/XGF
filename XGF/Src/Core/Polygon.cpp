#include "../../Include/Polygon.hpp"
#include "../../Include/Logger.hpp"
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
		if(c)
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

	unsigned BindingBridge::GetAllPolygonPleMemSize() const
	{
		auto i = std::accumulate(binders.begin(), binders.end(), 0u, [](unsigned i, std::shared_ptr<PolygonPleBinder> binder)
		{
#ifdef _DEBUG
				if (binder->GetActualCount() == 0) 
				{
					XGF_Error(Render, "binder actual count is zero");
				}
#endif
			return binder->GetActualCount() * binder->SizeOf() + i;
		});
		return i;
	}

	////////////////////////////////////////////////////////////////////////////
	PolygonPleIndex::PolygonPleIndex(int n)
	{
		mIndex = new Index[n];
		mCount = 0;
		mCapacity = n;
	}
	Index PolygonPleIndex::Get(int n) const
	{
		return mIndex[n];
	}

	Index& PolygonPleIndex::Get(int n)
	{
		return mIndex[n];
	}

	void PolygonPleIndex::SetFullActualCount()
	{
		mCount = mCapacity;
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

	void PolygonPleIndex::SetActualCount(int count)
	{
		XGF_ASSERT(mCount <= mCapacity);
		mCount = count;
	}

	int PolygonPleIndex::GetActualCount() const
	{
		return mCount;
	}

	int PolygonPleIndex::GetCapacity() const
	{
		return mCapacity;
	}

	PolygonPleIndex::PolygonPleIndex(const PolygonPleIndex & tb)
	{
		if (&tb == this) return;
		this->mCount = tb.mCount;
		mIndex = new Index[mCount];
		memcpy(this->mIndex, tb.mIndex, sizeof(Index) * mCount);
	}

	PolygonPleBinder::PolygonPleBinder(int capacity): mCapacity(capacity)
	{
		
	}
	void PolygonPleBinder::SetActualCount(int c)
	{
		mCount = c;
	}
	int PolygonPleBinder::GetActualCount() const
	{
		return mCount;
	}
	int PolygonPleBinder::GetCapacity() const
	{
		return mCapacity;
	}
	void PolygonPleBinder::SetFullActualCount()
	{
		mCount = mCapacity;
	}
	void PolygonPleBinder::SetCapacity(int capacity)
	{
		mCapacity = capacity;
	}
}
