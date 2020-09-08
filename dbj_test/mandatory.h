#pragma once

/* (c) 2019-2020 by dbj.org   -- LICENSE DBJ -- https://dbj.org/license_dbj/ */
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include "dbj_nano_synchro.h" //  dbj::lock_unlock autolock_ ;

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

namespace detail {

	// https://tinyurl.com/y5zhw5x3
	// in english: 8
	// malloc aligns on this boundary
	constexpr auto default_alignment = sizeof(double);

	//  int posix_memalign(void **memptr, size_t alignment, size_t size);
	// we use Strong types concept
	typedef struct Alignment { size_t val; } Alignment;
	typedef struct Size { size_t val; } Size;

	// corecrt_malloc.h contains _aligned_malloc
	// since we use string types as arguments nobody can not make 
	// a very common mistake of swapping alignment and size
	inline int dbj_alloc_aligned(void** p, Alignment alignment_, Size size_) noexcept
	{
		DBJ_LOCAL_LOCK;

		int erc = 0;
		_set_errno(0);

#ifdef _WIN32
		* p = _aligned_malloc(size_.val, alignment_.val);
		erc = errno;
#else  // ! _WIN32
		erc = posix_memalign(p, size_.val, alignment_.val);
#endif // ! _WIN32
		return erc;
	}

	inline int dbj_free(void* p) noexcept
	{
		dbj::local_lock_unlock autolock_;

		int erc = 0;
		_set_errno(0);
#ifdef _WIN32
		_aligned_free(p);
		erc = errno;
#else  // ! _WIN32
		free(p);
		erc = errno;
#endif // ! _WIN32
		p = NULL;
		return erc;
	}
} // detail ns

#ifdef TEST_MALLOC_ALIGNED

inline int test_malloc_aligned() noexcept
{
    dbj::local_lock_unlock autolock_;

    printf("\n" VT100_LIGHT_BLUE __FUNCSIG__  VT100_RESET );

    char* mem = NULL;
    // CL does not do "compund literals"
    // so we do string types in a pedestrian way
    // that is we can not do
    // int erc = dbj_alloc_aligned((void**)&mem, (Alignment) { alignment }, (Size) { n });

	using namespace detail;

    Alignment alignment = { 64 };
    Size n = { 100 } ;

    int erc = dbj_alloc_aligned((void**)&mem, alignment , n );

    const size_t wrong_alignment = 13;

    printf("\nis %3zd byte aligned = %s", wrong_alignment, (((size_t)mem) % wrong_alignment) ? "no" : "yes");
    printf("\nis %3zd byte aligned = %s", alignment.val, (((size_t)mem) % alignment.val ) ? "no" : "yes");

    dbj_free(mem);

    return EXIT_SUCCESS ;
}

#endif // TEST_MALLOC_ALIGNED

/*******************************************************************************************
here we start definining our own alloc / dealloc functions
NOTE: they are always required
NOTE: yes they will be simplified
*/


namespace eastl {
	extern "C" {

		inline void* user_defined_alloc(
			size_t size_,
			const char* pName,
			int flags,
			unsigned debugFlags,
			const char* file,
			int line
		) noexcept
		{
			void* p_{};
			constexpr detail::Alignment al_{ detail::default_alignment };
			detail::Size sz_{ size_ };
			int erc_ = detail::dbj_alloc_aligned(&p_, al_, sz_);
			EASTL_ASSERT( erc_ > 0  );
			return p_;
		}

		// alligned allocation
		inline void* user_defined_alloc_aligned(
			size_t size_,
			size_t alignment_,
			size_t alignmentOffset,
			const char* pName,
			int flags,
			unsigned debugFlags,
			const char* file,
			int line
		) noexcept
		{
			void* p_{};
			detail::Alignment al_{ alignment_ };
			detail::Size sz_{ size_ };
			int erc_ = detail::dbj_alloc_aligned(&p_, al_, sz_);
			EASTL_ASSERT(erc_ > 0);
			return p_;
		}

		// dbj eastl2010 change to allow for default allocator
		// to use matching de allocation / allocation
		// just like in this case
		inline void user_defined_deallocate(void* ptr_) noexcept
		{
			 detail::dbj_free(ptr_);
		}
	} // "C"
} // eastl ns



#ifdef TEST_CUSTOM_ALLOCATOR
#include <EASTL/string.h>

inline int test_custom_allocator()
{
    printf("\n" VT100_LIGHT_BLUE __FUNCSIG__  VT100_RESET);
  eastl::string str;

  str += "o";
  str += "m";
  str += "f";
  str += "g";
  str += "o";
  str += "m";
  str += "f";
  str += "g";
  str += "o";
  str += "m";
  str += "f";
  str += "g";
  str += "o";
  str += "m";
  str += "f";
  str += "g";
  str += "o";
  str += "m";
  str += "f";
  str += "g";
  str += "o";
  str += "m";
  str += "f";
  str += "g";
  // EASTL_ASSERT(str == "omfg");

  return EXIT_SUCCESS;
}

#endif // TEST_CUSTOM_ALLOCATOR

