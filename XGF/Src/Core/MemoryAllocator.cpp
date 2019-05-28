#include "../../Include/MemoryAllocator.hpp"


namespace XGF
{
	void* FrameMemoryAllocator::NewMem(size_t size)
	{
		// align 16 bytes
		size = (size + 16 - 1) & ~(16 - 1);
		for (auto& it : blocks)
		{
			if (it.GetRestSize() >= size)
			{
				return it.GetAvailableMemory(size);
			}
		}
		size_t alloc_size = size;
		if (alloc_size < minAllocSize)
		{
			alloc_size = minAllocSize;
		}
		return blocks.emplace_back(alloc_size).GetAvailableMemory(size);
	}
	void FrameMemoryAllocator::FreeAll()
	{
		for (auto& it : blocks)
		{
			it.ClearRestSize();
		}
	}
	FrameMemoryAllocator::FrameMemoryBlock::FrameMemoryBlock(size_t s): size(s), rest(s)
	{
		rawStart = (char*)malloc(s + 16);
		// align 16 bytes
		start = (char *) (((size_t)rawStart + 16 - 1) & ~(16 - 1));
	}
	inline FrameMemoryAllocator::FrameMemoryBlock::~FrameMemoryBlock()
	{
		free(rawStart);
	}
	size_t FrameMemoryAllocator::FrameMemoryBlock::GetRestSize() const
	{
		return rest;
	}
	size_t FrameMemoryAllocator::FrameMemoryBlock::GetUsedSize() const
	{
		return size - rest;
	}
	void FrameMemoryAllocator::FrameMemoryBlock::ClearRestSize()
	{
		rest = size;
	}
	void* FrameMemoryAllocator::FrameMemoryBlock::GetAvailableMemory(size_t s)
	{
		void* m = start + (size - rest);
		rest -= s;
		return m;
	}
}