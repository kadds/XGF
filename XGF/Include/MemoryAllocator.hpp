#pragma once
#include "Defines.hpp"
#include <list>
#ifdef new
#undef new
#endif
namespace XGF
{
	class FrameMemoryAllocator
	{
		// 2 MB
		static inline const int minAllocSize = 1 << 21;
		class FrameMemoryBlock
		{
		private:
			char* start;
			char* rawStart;
			size_t rest;
			const size_t size;
		public:
			FrameMemoryBlock(size_t size);
			~FrameMemoryBlock();
			size_t GetRestSize() const;
			size_t GetUsedSize() const;
			void ClearRestSize();
			void* GetAvailableMemory(size_t s);
		};

		std::list<FrameMemoryBlock> blocks;
		DisableCopyAndAssign(FrameMemoryAllocator);
	public:
		FrameMemoryAllocator() = default;
		void* NewMem(size_t size);
		template<typename T, typename ...Args>
		T* New(Args&& ... args)
		{
			T* ptr = (T*)NewMem(sizeof(T));
			return new (ptr) T(std::forward<Args>(args)...);
		}
		template<typename T>
		T* NewArray(size_t count)
		{
			char* mem = (char*)NewMem(sizeof(T) * count);
			for (int i = 0; i < count; i++)
			{
				T* ptr = (T*)(mem + sizeof(T) * i);
				new (ptr) T;
			}
			return (T*)mem;
		}
		void FreeAll();
	};

	template<typename T>
	class FrameMemorySTLAllocator {
	public:

		typedef size_t     size_type;
		typedef ptrdiff_t  difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T          value_type;
		template<typename X>
		struct rebind
		{
			typedef FrameMemorySTLAllocator<X> other;
		};

		FrameMemorySTLAllocator() throw() {
		}

		FrameMemorySTLAllocator(const FrameMemorySTLAllocator&) throw() {
		}
		template<typename X>
		FrameMemorySTLAllocator(const FrameMemorySTLAllocator<X>&) throw() {
		}
		~FrameMemorySTLAllocator() throw() {
		};
		pointer address(reference __x) const { return &__x; }

		const_pointer address(const_reference __x) const { return &__x; }

		pointer allocate(size_type __n, const void* hint = 0) {
			return reinterpret_cast<T*>(Context::Current().QueryFrameAllocator().NewMem(__n * sizeof(T)));
		}
		void deallocate(pointer __p, size_type __n) {
		}
		size_type max_size() const throw() {
			return size_t(-1) / sizeof(T);
		}
		void construct(pointer __p, const T& __val) {
			::new(__p) T(__val);
		}
		void destroy(pointer __p) {
			__p->~T();
		}
	};
};
